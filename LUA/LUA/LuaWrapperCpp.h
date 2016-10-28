#ifndef LUA_WARPPER_CPP_H
#define LUA_WARPPER_CPP_H


extern "C"
{
	#include "./lua_lib/lua.h"
	#include "./lua_lib/lualib.h"
	#include "./lua_lib/lauxlib.h"
}

#include "./LuaMacros.h"

#include "./LuaWrapper.h"
#include "./LuaScript.h"
#include "./LuaClassBind.h"
#include "./LuaCallbacks.h"



#endif
