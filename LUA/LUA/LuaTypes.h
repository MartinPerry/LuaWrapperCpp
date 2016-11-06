#ifndef LUA_TYPES_H
#define LUA_TYPES_H

#include "./LuaMacros.h"

//forward declarations

struct lua_State;

//=============================================================================================
// Other structures
//#include "../Strings/MyStringAnsi.h"
#include <string>
#include <functional>


typedef LUA_STRING LuaString;

typedef enum AttrCallType { GET, SET } AttrCallType;

typedef int(*getSetFunction) (lua_State *L, AttrCallType type);

typedef struct luaL_RegAttr {
	const char *name;
	getSetFunction func;
} luaL_RegAttr;

typedef std::function<LuaString(const LuaString & filePath)> ScriptLoaderCallback;

#endif