#include "./LuaCallbacks.h"

std::unordered_map<MyConstString, std::function<void*(Lua::LuaScript *)> > LuaCallbacks::ctors;
std::unordered_map<MyConstString, std::function<std::string(void *)> > LuaCallbacks::toString;


static void stackDump(lua_State *L) {
	int i = lua_gettop(L);
	printf(" ----------------  Stack Dump ----------------\n");
	while (i) {
		int t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			printf("%d:'%s'\n", i, lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf("%d: %s\n", i, lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("%d: %g\n", i, lua_tonumber(L, i));
			break;		
		default: printf("%d: %s\n", i, lua_typename(L, t)); break;
		}
		i--;
	}
	printf("--------------- Stack Dump Finished ---------------\n");
}


static void iterate_and_print(lua_State *L, int index)
{
	// Push another reference to the table on top of the stack (so we know
	// where it is, and this function can work for negative, positive and
	// pseudo indices
	lua_pushvalue(L, index);
	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2))
	{
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		const char *key = lua_tostring(L, -1);
		const char *value = lua_tostring(L, -2);
		printf("%s => %s\n", key, value);
		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function
}

int LuaCallbacks::tmp(lua_State * L)
{
	//Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	
	const char * keyName = luaL_checkstring(L, -1);
	const char * argsMetatableName = lua_tostring(L, lua_upvalueindex(1));
		
	luaL_getmetatable(L, argsMetatableName);	
	lua_getfield(L, -1, keyName);	
	lua_CFunction getArg = (lua_CFunction)lua_touserdata(L, -1);
	
	if (getArg == NULL)
	{
		printf("????");
		return 1;
	}

	luaL_getmetatable(L, argsMetatableName);
	lua_getfield(L, -1, "__parent");
	void *a = lua_touserdata(L, -1);

		
	//script->PrintStack();	
	lua_settop(L, 0); //clear stack
		
	lua_pushlightuserdata(L, a);
	//lua_pushnumber(L, 126);
	
	//script->PrintStack();

	
	getArg(L);
	
	

	return 1;
}