#include "Utils.h"
#include "JavaClass.h"


JType* cloneValue(JType* value) {
     if (value == nullptr) {
         return nullptr;
     }
     JType* dupvalue{};
     if (typeid(*value) == typeid(JDouble)) {
         dupvalue = new JDouble();
         dynamic_cast<JDouble*>(dupvalue)->val = dynamic_cast<JDouble*>(value)->val;
     }
     else if (typeid(*value) == typeid(JFloat)) {
         dupvalue = new JFloat();
         dynamic_cast<JFloat*>(dupvalue)->val = dynamic_cast<JFloat*>(value)->val;
     }
     else if (typeid(*value) == typeid(JInt)) {
         dupvalue = new JInt();
         dynamic_cast<JInt*>(dupvalue)->val = dynamic_cast<JInt*>(value)->val;
     }
     else if (typeid(*value) == typeid(JLong)) {
         dupvalue = new JDouble();
         dynamic_cast<JDouble*>(dupvalue)->val = dynamic_cast<JDouble*>(value)->val;
     }
     else if (typeid(*value) == typeid(JObject)) {
         dupvalue = new JObject();
         dynamic_cast<JObject*>(dupvalue)->jc = dynamic_cast<JObject*>(value)->jc;
         dynamic_cast<JObject*>(dupvalue)->offset = dynamic_cast<JObject*>(value)->offset;
     }
     else if (typeid(*value) == typeid(JArray)) {
         dupvalue = new JArray();
         dynamic_cast<JArray*>(dupvalue)->length = dynamic_cast<JArray*>(value)->length;
         dynamic_cast<JArray*>(dupvalue)->offset = dynamic_cast<JArray*>(value)->offset;
     }
     else {
         SHOULD_NOT_REACH_HERE
     }
     return dupvalue;
}

bool hasInheritanceRelationship(const JavaClass* source, const JavaClass* super) {
    if(strcmp((char*)source->getClassName(),(char*)super->getClassName())==0) {
        return true;
    }
    if(source->hasSuperClass()) {
        return hasInheritanceRelationship(yrt.ma->loadClassIfAbsent((char*)source->getSuperClassName()), super);
    }
    return false;
}
