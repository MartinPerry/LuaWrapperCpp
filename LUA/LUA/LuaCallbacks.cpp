#include "./LuaCallbacks.h"

std::unordered_map<std::type_index, MyStringAnsi > LuaCallbacks::tableName;
std::unordered_map<std::type_index, std::function<void*(Lua::LuaScript *)> > LuaCallbacks::ctors;
std::unordered_map<std::type_index, std::function<std::string(void *)> > LuaCallbacks::toString;


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


#include "../Strings/MyString.h"
#include "../TestClass.h"

int LuaCallbacks::tmp3(lua_State * L)
{
	Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);


	Account * newData = new Account(0);

	
	const char * classTableName = "Account";

	MyStringAnsi argsMetatable = classTableName;
	argsMetatable += "_attrs";
	const char * classArgsTableName = argsMetatable.c_str();

	Account ** udata = (Account **)lua_newuserdata(L, sizeof(Account *));
	*udata = newData;


	printf("New data: %p %p\n", udata, *udata);

	//put the "pointer to data" into "arguments" table	
	/*
	luaL_getmetatable(L, classArgsTableName);
	lua_pushstring(L, "__parent");
	lua_pushlightuserdata(L, *udata);
	lua_rawset(L, -3);

	lua_pop(L, 1);
	*/
	
	lua_settop(L, 0);

	

	

	//lua_getmetatable(L, 1);

	script->PrintStack("sss");

	lua_newtable(L); //mt1
	lua_pushstring(L, "__data");
	lua_pushlightuserdata(L, *udata);
	lua_settable(L, -3);

	luaL_getmetatable(L, classTableName);
	script->PrintStack("x");

	lua_setmetatable(L, -2);

	return 1;
}


int LuaCallbacks::tmp2(lua_State * L, const char * metatableName)
{
	

	Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	
	
	
	lua_insert(L, 1);

	const char * keyName = luaL_checkstring(L, -1);

	luaL_getmetatable(L, metatableName);
	lua_getfield(L, -1, keyName);
	getSetFunction getSetArg = (getSetFunction)lua_touserdata(L, -1);

	if (getSetArg == NULL)
	{
		printf("????");
		return 1;
	}

	
	lua_settop(L, 2);  //remove everything	except last 2 values from stack
	lua_insert(L, 1);  //"put" stack top value on stack bottom
	//script->PrintStack("xxx");

	/*
	luaL_getmetatable(L, argsMetatableName);
	lua_getfield(L, -1, "__parent");
	//void *a = lua_touserdata(L, -1);

	lua_insert(L, 1);	
	lua_pop(L, 1);
	*/
	//lua_pushlightuserdata(L, a);
	//lua_pushvalue(L, 1);

	

	getSetArg(L, AttrCallType::SET);

	return 0;
}

int LuaCallbacks::tmp(lua_State * L, const char * metatableName)
{
	

	Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	
	
	const char * keyName = luaL_checkstring(L, -1);	
	luaL_getmetatable(L, metatableName);	
	lua_getfield(L, -1, keyName);		
	getSetFunction getSetArg = (getSetFunction)lua_touserdata(L, -1);
	
	if (getSetArg == NULL)
	{		
		return 1;
	}
	/*
	luaL_getmetatable(L, argsMetatableName); //set "arguments" metatable on stack top
	lua_getfield(L, -1, "__parent"); //get "__parent" value on top

	lua_insert(L, 1); //"put" "__parent" value on stack bottom
	lua_settop(L, 1); //remove everything	except last value from stack
	*/
	getSetArg(L, AttrCallType::GET);
	
	

	return 1;
}