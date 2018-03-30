#include "MethodArea.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "AccessFlag.h"
#include "Descriptor.h"
#include "Opcode.h"


MethodArea::MethodArea(const char *searchPath[],int howManySearchPath) {
    for(int i=0;i<howManySearchPath;i++) {
        searchPaths.emplace_back(searchPath[i]);
    }
}

MethodArea::~MethodArea() {
    for (auto& x : classTable) {
        delete x.second;
    }
}

JavaClass * MethodArea::findJavaClass(const char* jcName) {
    const auto pos = classTable.find(jcName);
    if (pos != classTable.end()) {
        return pos->second;
    }
    else {
        return nullptr;
    }
}

bool MethodArea::loadJavaClass(const char * jcName) {
    auto path = parseNameToPath(jcName);

    if (path.length() != 0 && !findJavaClass(jcName)) {
        // Load this class which specified by jcName (it' a path string)
        JavaClass * jc = new JavaClass(path.c_str());
        jc->parseClassFile();
        classTable.insert(std::make_pair(std::string((char*)jc->getClassName()), jc));

        // Load super class if it doesnt exist in class table
        if (jc->getSuperClassName() && !findJavaClass((char*)jc->getSuperClassName())) {
            this->loadJavaClass((char*)jc->getSuperClassName());
        }

        // Load super interfaces if exitsted
        std::vector<u2> && interfacesIdx = jc->getInterfacesIndex();
        if (jc->getInterfacesIndex().empty()) {
            for (auto idx : interfacesIdx) {
                this->loadJavaClass((char *)jc->getString(idx));
            }
        }

        return true;
    }
    return false;
}

void MethodArea::linkJavaClass(const char * jcName) {
    JavaClass * javaClass = yrt.ma->findJavaClass(jcName);
    FOR_EACH(fieldOffset, javaClass->raw.fieldsCount) {
        char * descriptor = (char*)javaClass->getString(javaClass->raw.fields[fieldOffset].descriptorIndex);
        if (IS_FIELD_REF_CLASS(descriptor)) {
            // Special handling for field whose type is another class
            if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
                JObject * fieldObject = nullptr;

                FOR_EACH(fieldAttr, javaClass->raw.fields[fieldOffset].attributeCount) {
                    if (typeid(*javaClass->raw.fields[fieldOffset].attributes[fieldAttr]) == typeid(ATTR_ConstantValue)) {
                        if (strcmp("Ljava/lang/String;", descriptor) == 0) {
                            const char * constantStr = (char*)javaClass->getString(
                                ((CONSTANT_String*)javaClass->raw.constPoolInfo[
                                    ((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr]
                                        )->constantValueIndex])->stringIndex);
                            int strLen = strlen(constantStr);
                            fieldObject = yrt.jheap->createObject(*yrt.ma->loadClassIfAbsent("java/lang/String"));
                            yrt.jheap->putObjectField(*fieldObject, 0, yrt.jheap->createCharArray(constantStr, strLen));
                        }
                    }
                }

                javaClass->sfield.insert(std::make_pair(fieldOffset, fieldObject));
            }
        }
        else if (IS_FIELD_REF_ARRAY(descriptor)) {
            // Special handling for field whose type is array. We create a null JArray as a placeholder
            // since we dont know more information about size of array, so we defer to allocate memory
            // while meeting opcodes [newarray]/[multinewarray]

            if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
                JArray * uninitializedArray = nullptr;
                javaClass->sfield.insert(std::make_pair(fieldOffset, uninitializedArray));
            }
        }
        else {
            // Otherwise it's a basic type. We insert it into instance's field or its class static field
            // by determining its access flag
            if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
                JType * basicField = determineBasicType(descriptor);

                FOR_EACH(fieldAttr, javaClass->raw.fields[fieldOffset].attributeCount) {
                    if (typeid(*javaClass->raw.fields[fieldOffset].attributes[fieldAttr]) == typeid(ATTR_ConstantValue)) {
                        if (typeid(*javaClass->raw.constPoolInfo[
                            ((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex]) == typeid(CONSTANT_Long)) {
                            ((JLong*)basicField)->val = ((CONSTANT_Long*)javaClass->raw.constPoolInfo[((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex])->val;
                        }
                        else if (typeid(*javaClass->raw.constPoolInfo[
                            ((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex]) == typeid(CONSTANT_Double)) {
                            ((JDouble*)basicField)->val = ((CONSTANT_Double*)javaClass->raw.constPoolInfo[((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex])->val;
                        }
                        else if (typeid(*javaClass->raw.constPoolInfo[
                            ((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex]) == typeid(CONSTANT_Float)) {
                            ((JFloat*)basicField)->val = ((CONSTANT_Float*)javaClass->raw.constPoolInfo[((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex])->val;
                        }
                        else if (typeid(*javaClass->raw.constPoolInfo[
                            ((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex]) == typeid(CONSTANT_Integer)) {
                            ((JInt*)basicField)->val = ((CONSTANT_Integer*)javaClass->raw.constPoolInfo[((ATTR_ConstantValue*)javaClass->raw.fields[fieldOffset].attributes[fieldAttr])->constantValueIndex])->val;
                        }
                        else {
                            SHOULD_NOT_REACH_HERE
                        }
                    }
                }

                javaClass->sfield.insert(std::make_pair(fieldOffset, basicField));
            }
        }
    }
    linkedClasses.push_back((const char*)javaClass->getClassName());
}

void MethodArea::initJavaClass(CodeExecution & exec, const char * jcName){
    initedClasses.push_back(jcName);
    exec.invokeByName(yrt.ma->findJavaClass(jcName), "<clinit>", "()V");
}

bool MethodArea::removeJavaClass(const char * jcName) {
    auto pos = classTable.find(jcName);
    if (pos != classTable.end()) {
        classTable.erase(pos);
        return true;
    }
    return false;
}

std::string MethodArea::parseNameToPath(const char * name){
    using namespace std;
    // java/util/ArrayList
    string newJcName(name);
    for (auto & c : newJcName) {
        if (c == '/')
            c = '\\';
    }
    newJcName.append(".class");

    for (auto path : this->searchPaths) {
        if (path.length()>0 && path[path.length() - 1] != '\\') {
            path += '\\';
        }
        path += newJcName;
        fstream fin;
        fin.open(path, ios::in);
        if (fin) {
            fin.close();
            return path;    // Return only if it's a valid file path
        }
        fin.close();
    }
    return std::string("");
}
