#include "AccessFlag.h"
#include "Descriptor.h"
#include "JavaClass.h"
#include "MethodArea.h"
#include "Option.h"

MethodArea::MethodArea(const std::vector<std::string>& libPaths) {
	for (const auto& path : libPaths) {
		searchPaths.push_back(path);
	}
}

MethodArea::~MethodArea() {
	for (auto& x : classTable) {
		delete x.second;
	}
}

JavaClass* MethodArea::findJavaClass(const char* jcName) {
	std::lock_guard<std::recursive_mutex> lockMA(maMutex);

	const auto pos = classTable.find(jcName);
	if (pos != classTable.end()) {
		return pos->second;
	}
	return nullptr;
}

bool MethodArea::loadJavaClass(const char* jcName) {
	std::lock_guard<std::recursive_mutex> lockMA(maMutex);

	auto path = parseNameToPath(jcName);

	if (path.length() != 0 && !findJavaClass(jcName)) {
		// Load this class which specified by jcName (it' a path string)
		JavaClass* jc = new JavaClass(path.c_str());
		jc->parseClassFile();
		classTable.insert(std::make_pair(std::string(jc->getClassName()), jc));

		// Load super class if it doesnt exist in class table
		if (jc->getSuperClassName() && !findJavaClass(jc->getSuperClassName())) {
			this->loadJavaClass(jc->getSuperClassName());
		}

		// Load super interfaces if exitsted
		std::vector<u2>&& interfacesIdx = jc->getInterfacesIndex();
		if (jc->getInterfacesIndex().empty()) {
			for (auto idx : interfacesIdx) {
				this->loadJavaClass(jc->getString(idx));
			}
		}

		return true;
	}
	return false;
}

void MethodArea::linkJavaClass(const char* jcName) {
	std::lock_guard<std::recursive_mutex> lockMA(maMutex);

	JavaClass* javaClass = yrt.ma->findJavaClass(jcName);
	FOR_EACH(fieldOffset, javaClass->raw.fieldsCount) {
		const char* descriptor =
		    javaClass->getString(javaClass->raw.fields[fieldOffset].descriptorIndex);
		if (IS_FIELD_REF_CLASS(descriptor)) {
			// Special handling for field whose type is another class
			if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
				JObject* fieldObject = nullptr;

				FOR_EACH(fieldAttr, javaClass->raw.fields[fieldOffset].attributeCount) {
					if (typeid(*javaClass->raw.fields[fieldOffset].attributes[fieldAttr]) == typeid(ATTR_ConstantValue)) {
						if (strcmp("Ljava/lang/String;", descriptor) == 0) {
							const char* constantStr = javaClass->getString(
							    ((CONSTANT_String*) javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
							                                                          ->raw
							                                                          .fields[fieldOffset]
							                                                          .attributes[fieldAttr])
							                                                         ->constantValueIndex])
							        ->stringIndex);
							int strLen = strlen(constantStr);
							fieldObject = yrt.jheap->createObject(*yrt.ma->loadClassIfAbsent("java/lang/String"));
							yrt.jheap->putObjectFieldByOffset(*fieldObject, 0,
							                                  yrt.jheap->createCharArray(constantStr, strLen));
						}
					}
				}

				javaClass->sfield.insert(std::make_pair(fieldOffset, fieldObject));
			}
		} else if (IS_FIELD_REF_ARRAY(descriptor)) {
			// Special handling for field whose type is array. We create a null JArray as a placeholder
			// since we dont know more information about size of array, so we defer to allocate memory
			// while meeting opcodes [newarray]/[multinewarray]

			if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
				JArray* uninitializedArray = nullptr;
				javaClass->sfield.insert(std::make_pair(fieldOffset, uninitializedArray));
			}
		} else {
			// Otherwise it's a basic type. We insert it into instance's field
			// or its class static field by determining its access flag
			if (IS_FIELD_STATIC(javaClass->raw.fields[fieldOffset].accessFlags)) {
				JType* basicField = determineBasicType(descriptor);

				FOR_EACH(fieldAttr, javaClass->raw.fields[fieldOffset].attributeCount) {
					if (typeid(*javaClass->raw.fields[fieldOffset].attributes[fieldAttr]) == typeid(ATTR_ConstantValue)) {
						if (typeid(*javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
						                                              ->raw
						                                              .fields[fieldOffset]
						                                              .attributes[fieldAttr])
						                                             ->constantValueIndex]) == typeid(CONSTANT_Long)) {
							((JLong*) basicField)->val =
							    ((CONSTANT_Long*) javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
							                                                        ->raw
							                                                        .fields[fieldOffset]
							                                                        .attributes[fieldAttr])
							                                                       ->constantValueIndex])
							        ->val;
						} else if (typeid(*javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
						                                                     ->raw
						                                                     .fields[fieldOffset]
						                                                     .attributes[fieldAttr])
						                                                    ->constantValueIndex]) == typeid(CONSTANT_Double)) {
							((JDouble*) basicField)->val =
							    ((CONSTANT_Double*) javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
							                                                          ->raw
							                                                          .fields[fieldOffset]
							                                                          .attributes[fieldAttr])
							                                                         ->constantValueIndex])
							        ->val;
						} else if (typeid(*javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
						                                                     ->raw
						                                                     .fields[fieldOffset]
						                                                     .attributes[fieldAttr])
						                                                    ->constantValueIndex]) == typeid(CONSTANT_Float)) {
							((JFloat*) basicField)->val =
							    ((CONSTANT_Float*) javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
							                                                         ->raw
							                                                         .fields[fieldOffset]
							                                                         .attributes[fieldAttr])
							                                                        ->constantValueIndex])
							        ->val;
						} else if (typeid(*javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
						                                                     ->raw
						                                                     .fields[fieldOffset]
						                                                     .attributes[fieldAttr])
						                                                    ->constantValueIndex]) == typeid(CONSTANT_Integer)) {
							((JInt*) basicField)->val =
							    ((CONSTANT_Integer*) javaClass->raw.constPoolInfo[((ATTR_ConstantValue*) javaClass
							                                                           ->raw
							                                                           .fields[fieldOffset]
							                                                           .attributes[fieldAttr])
							                                                          ->constantValueIndex])
							        ->val;
						} else {
							SHOULD_NOT_REACH_HERE
						}
					}
				}

				javaClass->sfield.insert(std::make_pair(fieldOffset, basicField));
			}
		}
	}
	linkedClasses.insert(javaClass->getClassName());
}

void MethodArea::initJavaClass(CodeExecution& exec, const char* jcName) {
	std::lock_guard<std::recursive_mutex> lockMA(maMutex);
	initedClasses.insert(jcName);
	exec.invokeByName(yrt.ma->findJavaClass(jcName), "<clinit>", "()V");
}

bool MethodArea::removeJavaClass(const char* jcName) {
	std::lock_guard<std::recursive_mutex> lockMA(maMutex);

	auto pos = classTable.find(jcName);
	if (pos != classTable.end()) {
		classTable.erase(pos);
		return true;
	}
	return false;
}

std::string MethodArea::parseNameToPath(const char* name) {
	using namespace std;
	// java/util/ArrayList
	string newJcName(name);
	for (auto& c : newJcName) {
		if (c == '/')
#if defined(TARGET_WIN32)
			c = '\\';
#elif defined(TARGET_LINUX)
			c = '/';
#endif
		;
	}
	newJcName.append(".class");

	for (auto path : this->searchPaths) {
#if defined(TARGET_WIN32)
		if (path.length() > 0 && path[path.length() - 1] != '\\') {
			path += '\\';
		}
#elif defined(TARGET_LINUX)
		if (path.length() > 0 && path[path.length() - 1] != '/') {
			path += '/';
		}
#endif

		path += newJcName;
		fstream fin;
		fin.open(path, ios::in);
		if (fin) {
			fin.close();
			return path; // Return only if it's a valid file path
		}
		fin.close();
	}
	return std::string("");
}
