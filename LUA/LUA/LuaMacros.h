#ifndef LUA_MACROS_H
#define LUA_MACROS_H

//=============================================================================================
// Compilation defines

//Enable more type checking when getting pointer from Lua
//However, this is a bit slower then unsafe version without checks
#define SAFE_PTR_CHECKS	1

//String class used in Lua
#define LUA_STRING std::string

//inline call
#define LUA_INLINE __forceinline 

//FORCE_INLINE

//=============================================================================================
// Some helper DEFINEs

//Macro for registration of overloaded methods
#define CLASS_METHOD_OVERLOAD(ClassName, MethodName, ...) \
	LuaCallbacks::function<decltype(ClassOverloadMethod<ClassName, ##__VA_ARGS__>::get(&ClassName::MethodName)), &ClassName::MethodName>

//Macro for registration of class methods
#define CLASS_METHOD(ClassName, MethodName) \
	LuaCallbacks::function<decltype(&ClassName::MethodName), &ClassName::MethodName>

//Macro for class attribute registration
#define CLASS_ATTRIBUTE(ClassName, AttrName) \
	LuaCallbacks::getSetAttr<decltype(ClassName::AttrName), ClassName, &ClassName::AttrName>

//Macro for simple method registration
#define METHOD(MethodName) \
	LuaCallbacks::function<decltype(&MethodName), &MethodName>

#define LUA_SAFE_DELETE(a) {if (a != NULL) { delete a; a = NULL; }};
#define LUA_SAFE_DELETE_ARRAY(a) {if (a != NULL) { delete[] a; a = NULL; }};
//=============================================================================================
// Debug logging


#ifndef LUA_LOG_ERROR
	#define LUA_LOG_ERROR(...) printf(__VA_ARGS__)
#endif

#ifndef LUA_LOG_WARNING
	#define LUA_LOG_WARNING(...) printf(__VA_ARGS__)
#endif

#ifndef LUA_LOG_INFO
	#define LUA_LOG_INFO(...) printf(__VA_ARGS__)
#endif




#endif
