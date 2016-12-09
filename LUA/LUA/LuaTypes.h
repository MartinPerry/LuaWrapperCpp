#ifndef LUA_TYPES_H
#define LUA_TYPES_H

#include "./LuaMacros.h"

//forward declarations

struct lua_State;

namespace Lua
{
	class LuaScript;
}
//=============================================================================================
// Other structures

#include <string>

#include <functional>
#include <memory>


typedef LUA_STRING LuaString;

typedef enum AttrCallType { GET, SET } AttrCallType;

typedef int(*getSetFunction) (lua_State *L, AttrCallType type);

typedef struct luaL_RegAttr {
	const char *name;
	getSetFunction func;
} luaL_RegAttr;

typedef std::function<LuaString(const LuaString & filePath)> ScriptLoaderCallback;
typedef std::function<void(std::shared_ptr<Lua::LuaScript> script )> RegisterCallback;

template <typename T>
using LuaToStringCallback = std::function<LuaString(T *)>;

struct LuaClassToString
{
	virtual LuaString Call(void * ptr) const = 0;	
};

#endif