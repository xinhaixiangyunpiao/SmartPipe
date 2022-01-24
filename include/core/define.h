#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <map>
#include <string>
#include <vector>
#include <cassert>

// 声明具有动态创建的基类
#define DECLEAR_DYNCRT_BASE(base) \
public: \
typedef base* (*ClassGen)(std::vector<std::string>&); \
static void _register(std::string class_name, ClassGen class_gen) \
{ \
    class_set.insert(std::map<std::string, ClassGen>::value_type(class_name, class_gen)); \
} \
static base* create(std::string class_name, std::vector<std::string>& v) \
{ \
    std::map<std::string, ClassGen>::iterator it = class_set.find(class_name);   \
    if (it != class_set.end()) \
        return (it->second)(v); \
    assert(false); \
    return NULL; \
} \
protected: \
static std::map<std::string, ClassGen> class_set;

// 用于实现基类
#define IMPLEMENT_DYNCRT_BASE(base) \
std::map<std::string, base::ClassGen> base::class_set;

// 用于声明一个能够被动态创建的类(用一个全局对象进行注册)
#define DECLEAR_DYNCRT_CLASS(id, derived, base) \
public: \
    struct derived##register \
    { \
        derived##register() \
        { \
            static bool bRegister = false; \
            if (!bRegister) \
            { \
                base::_register(id, _CreateObjectByName); \
                bRegister = true; \
            } \
        } \
    }; \
    static base* _CreateObjectByName(std::vector<std::string>& v) \
    { \
        return new derived(v); \
    }

// 用于实现一个能被动态创建的类
#define IMPLEMENT_DYNCRT_CLASS(derived) \
    static derived::derived##register _##derived##global_object;

#endif