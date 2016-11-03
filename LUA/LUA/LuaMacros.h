#ifndef LUA_MACROS_H
#define LUA_MACROS_H

//=============================================================================================
// Compilation defines

//Enable more type checking when getting pointer from Lua
//However, this is a bit slower then unsafe version without checks
#define SAFE_PTR_CHECKS	1


#define LUA_INLINE FORCE_INLINE


//=============================================================================================
// Some helper DEFINEs

#define CLASS_METHOD(ClassName, MethodName) \
	LuaCallbacks::function<decltype(&ClassName::MethodName), &ClassName::MethodName>

#define METHOD(MethodName) \
	LuaCallbacks::function<decltype(&MethodName), &MethodName>

#define CLASS_ATTRIBUTE(ClassName, AttrName) \
	LuaCallbacks::getSetAttr<decltype(ClassName::AttrName), ClassName, &ClassName::AttrName>



//=============================================================================================
// Other structures
#include "../Strings/MyStringAnsi.h"

typedef MyStringAnsi LuaString;

typedef enum AttrCallType { GET, SET } AttrCallType;

typedef int(*getSetFunction) (lua_State *L, AttrCallType type);

typedef struct luaL_RegAttr {
	const char *name;
	getSetFunction func;
} luaL_RegAttr;



#endif
