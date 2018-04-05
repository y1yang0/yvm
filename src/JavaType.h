#ifndef YVM_JAVAOBJECT_H
#define YVM_JAVAOBJECT_H

#include <cstdint>
#include <atomic>
#include <thread>

class JavaClass;

// These types widely use in jvm its internal
struct JType {
    virtual ~JType() = default;
};

#define BASE_OF_JTYPE :public JType

struct JVoid BASE_OF_JTYPE {};

typedef JVoid JRef;

struct JDouble BASE_OF_JTYPE {
    explicit JDouble() = default;
    explicit JDouble(double val) : val(val) {}
    double val = 0.0;
};

struct JFloat BASE_OF_JTYPE {
    explicit JFloat() = default;
    explicit JFloat(float val) : val(val) {}
    float val = 0.0f;
};

struct JInt BASE_OF_JTYPE {
    explicit JInt() = default;
    explicit JInt(int32_t val) : val(val) {}
    int32_t val = 0;
};

struct JLong BASE_OF_JTYPE {
    explicit JLong() = default;
    explicit JLong(int64_t val) : val(val) {}
    int64_t val = 0L;
};

struct JObject BASE_OF_JTYPE {
    explicit JObject() { }

    size_t offset = 0; // Offset on java heap
    const JavaClass* jc{}; // Reference to meta java class
};

struct JArray BASE_OF_JTYPE {
    explicit JArray() { }

    int length = 0; // Length of java array
    size_t offset = 0; // Offset on java heap
};

#endif // !YVM_OBJECT_H
