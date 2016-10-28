#ifndef LUA_MACROS_H
#define LUA_MACROS_H

//=============================================================================================
// Compilation defines

#define SAFE_PTR_CHECKS	1
#define INLINE FORCE_INLINE


//=============================================================================================
// Some helper DEFINEs

#define CLASS_METHOD(ClassName, MethodName) \
	LuaCallbacks::function<decltype(&ClassName::MethodName), &ClassName::MethodName>

#define METHOD(MethodName) \
	LuaCallbacks::function<decltype(&MethodName), &MethodName>

#endif
