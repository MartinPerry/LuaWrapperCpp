#ifndef LUA_SCRIPT_H
	#include "LuaScript.h"
#endif

extern "C"
{
	#include "./lua_lib/lua.h"
	#include "./lua_lib/lualib.h"
	#include "./lua_lib/lauxlib.h"
}


#include "../Strings/MyConstString.h"

/*-----------------------------------------------------------
Function:	Register
Parameters:

Returns:


Register new lua binding for class
-------------------------------------------------------------*/
template <typename T>
void LuaScript::RegisterClass(const LuaClassBind<T> & classBind)
{
	MyConstString key = MyConstString(typeid(T).name());

	if (LuaCallbacks::ctors.find(key) != LuaCallbacks::ctors.end())
	{
		MY_LOG_ERROR("Class %s already registered", key.GetConstString());
		return;
	}

	LuaCallbacks::ctors[key] = classBind.ctor;
	LuaCallbacks::toString[key] = classBind.toString;


	//http://cfc.kizzx2.com/index.php/binding-c-classes-to-lua-a-step-by-step-example-for-beginners/


	//http://lua-users.org/lists/lua-l/2006-08/msg00245.html

	lua_State * L = this->state;

		
	luaL_newmetatable(L, classBind.className.c_str());
	int metaTableID = lua_gettop(L);

	// Register the C functions _into_ the metatable we just created.
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classBind.className.c_str());
	luaL_setfuncs(L, &classBind.methods[0], 1);
#else
	luaL_setfuncs(L, &classBind.methods[0], 0);
#endif
		
	lua_pushvalue(L, -1);	
	lua_setfield(L, -1, "__index");

	this->PrintStack("METATABLE");
	
	lua_pushstring(L, classBind.className.c_str());	
	lua_pushcclosure(L, LuaCallbacks::create_new<T>, 1);
	lua_setglobal(L, classBind.className.c_str()); // this is how function will be named in Lua
	
	//double underscore variables
	//http://nova-fusion.com/2011/06/30/lua-metatables-tutorial/
	//http://stackoverflow.com/questions/10891957/difference-between-tables-and-metatables-in-lua


	//rewrite __gc with our own
	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, LuaCallbacks::garbage_collect<T>);	
	lua_settable(L, metaTableID);

	

	lua_pushliteral(L, "__tostring");
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classBind.className.c_str());	
	lua_pushcclosure(L, LuaCallbacks::to_string<T>, 1);
#else
	lua_pushcclosure(L, LuaCallbacks::to_string<T>, 0);
#endif
	lua_settable(L, metaTableID);

			
	//return;
	//---------------------------------------------------------------------

	lua_pop(L, -1);
	luaL_getmetatable(L, classBind.className.c_str());

	MyStringAnsi cc = classBind.className;
	cc += "_atrs";
	luaL_newmetatable(L, cc.c_str()); //mt1
	int tableID = lua_gettop(L);
					 // push a temporary variable into the imt
	

	//this->PrintStack("Bef");
	this->PrintStack("BEF");

	lua_pushliteral(L, "__index");
#if (SAFE_PTR_CHECKS == 1)	
	lua_pushstring(L, cc.c_str());
	lua_pushstring(L, classBind.className.c_str());
	lua_pushcclosure(L, LuaCallbacks::index<T>, 2);
#else
	lua_pushcclosure(L, LuaCallbacks::index<T>, 0);
#endif	
	lua_settable(L, tableID);
	this->PrintStack("Aft");
	

	
	// set mt1.__index = mt1
	//lua_pushstring(L, "__index");
	//lua_pushvalue(L, -2);
	//lua_settable(L, -3);
	

	// set mt1.__newindex = mt1
	lua_pushstring(L, "__newindex");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_setmetatable(L, -2);		// setmetatable(mt2, mt1)

	this->PrintStack("shoda");
	for (size_t i = 0; i < classBind.attrs.size() - 1; i++)
	{
		//lua_pushlightuserdata(L, classBind.attrs[i].func);
		//lua_setfield(L, -2, classBind.attrs[i].name);

		lua_pushstring(L, classBind.attrs[i].name);
		lua_pushlightuserdata(L, classBind.attrs[i].func);
		lua_settable(L, -3);
	}

	//lua_pushstring(L, "instancevar");
	//lua_pushnumber(L, 0.5);
	//lua_settable(L, -3);

	// get class metatable (mt2) & assign to instance metatable (mt1)
	
	/*
	this->PrintStack("setmeta0");
	//lua_settable(L, tableID);
	lua_pop(L, -1);
	luaL_getmetatable(L, classBind.className.c_str());
	lua_pushvalue(L, tableID);
	*/
	this->PrintStack("setmeta");
	
	

	

	return;


	lua_pop(L, -1);

	

	//http://stackoverflow.com/questions/20147027/creating-a-simple-table-with-lua-tables-c-api
	
	
	
	MyStringAnsi mm = classBind.className.c_str();
	mm += "_attrs";
	luaL_newmetatable(L, mm.c_str());
	int metaTableIDmm = lua_gettop(L);

	this->PrintStack("METATABLE 2");

	lua_pushvalue(L, -1);	
	lua_setfield(L, -1, "__index");
	
		
	lua_pushliteral(L, "__index");	
#if (SAFE_PTR_CHECKS == 1)	
	lua_pushstring(L, classBind.className.c_str());				
	lua_pushcclosure(L, LuaCallbacks::index<T>, 1);	
#else
	lua_pushcclosure(L, LuaCallbacks::index<T>, 0);
#endif	
	lua_settable(L, metaTableIDmm);
	
	//stack should be empty
	this->PrintStack("EMPTY STACK");

	
	for (size_t i = 0; i < classBind.attrs.size() - 1; i++)
	{
		lua_pushlightuserdata(L, classBind.attrs[i].func);
		lua_setfield(L, -2, classBind.attrs[i].name);		
	}
	
	//--------------------------------
	lua_pop(L, -1);
	return;

	lua_createtable(L, 0, 1); // creates and pushes new table on top of Lua stack 

	this->PrintStack("TABLE");

	lua_pushstring(L, "aaa"); // Pushes table value on top of Lua stack 
	this->PrintStack("9");
	lua_setfield(L, -2, "vv");  // table["vv"] = "aaa". Pops key value 


	//lua_pop(L, -1);

	//stack should be empty
	//this->PrintStack("EMPTY STACK");

	
	luaL_getmetatable(L, mm.c_str());	
	luaL_getmetatable(L, classBind.className.c_str());

	this->PrintStack("ALL TABLES");

	lua_setmetatable(L, -2);  // Attach metatable.

	//lua_setfield(L, -1, "__index");
	

	/*
	lua_pushliteral(L, "__newindex");
	#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classBind.className.c_str());
	lua_pushcclosure(L, LuaCallbacks::new_index<T>, 1);
	#else
	lua_pushcclosure(L, LuaCallbacks::new_index<T>, 0);
	#endif
	lua_settable(L, metaTableID);
	*/


	/*
	lua_pushstring(L, "25.10");
	lua_setfield(L, -2, "date");

	lua_pushstring(L, "125.125");
	lua_setfield(L, -2, "ip");

	lua_pushstring(L, "ii");
	lua_setfield(L, -2, "custom");
	*/


//http://stackoverflow.com/questions/18478379/how-to-work-with-tables-passed-as-an-argument-to-a-lua-c-function
	/*
	lua_getfield(L, 1, "name");
	const char *nameO = luaL_checkstring(L, -1);

	//lua_getfield(L, -1, nameO);
	//const char *valO = luaL_checkstring(L, -2);

	//lua_rawgeti(L, -1, 1);
	//const char *valO = luaL_checkstring(L, -2);



	lua_pushstring(L, "name");  // now the top of the Lua stack is the string "bar"
								// one below the top of the stack is the table t
	lua_getmetatable(L, -2);       // the second element from top of stack is the table;
								   // now the top of stack is replaced by t["bar"]
	const char * x = lua_tostring(L, -3);   // convert value on the top of the stack to a C integer
											// so x should be 123 per your function
	lua_pop(L, 1);             // remove the value from the stack
	*/
	/*
	lua_pushstring(L, "name");
	lua_pushstring(L,  "aa");
	lua_settable(L, -3);  // 3rd element from the stack top

	lua_pushstring(L, "date");
	lua_pushstring(L, "25.10");
	lua_settable(L, -3);

	lua_pushstring(L, "ip");
	lua_pushstring(L, "252.25.2");
	lua_settable(L, -3);

	lua_pushstring(L, "custom");
	lua_pushstring(L, "xxx");
	lua_settable(L, -3);
	*/
	
}



//=============================================================================
//===================== Get data passed from Lua callback function ============
//=============================================================================


//template<>
//struct LuaScript::item_return<const MyStringAnsi &> { typedef MyStringAnsi type; };


template <typename T>
LUA_INLINE T * LuaScript::GetFnInputImpl(LuaScript::tag<T *>)
{	
	int argType = lua_type(this->state, this->stackPtr);

	if (argType == LUA_TUSERDATA)
	{
		T * a = (*(T **)(lua_touserdata(this->state, this->stackPtr++)));
		return a;
	}
	else if (argType == LUA_TLIGHTUSERDATA)
	{
		return static_cast<T *>(lua_touserdata(this->state, this->stackPtr++));
	}
	else
	{
		return NULL;
	}
};


LUA_INLINE bool LuaScript::GetFnInputImpl(LuaScript::tag<bool>)
{
	return (lua_toboolean(this->state, this->stackPtr++) == 1);
}


LUA_INLINE float LuaScript::GetFnInputImpl(LuaScript::tag<float>)
{
	return static_cast<float>(lua_tonumber(this->state, this->stackPtr++));
}

LUA_INLINE double LuaScript::GetFnInputImpl(LuaScript::tag<double>)
{
	return lua_tonumber(this->state, this->stackPtr++);
}


LUA_INLINE MyStringAnsi LuaScript::GetFnInputImpl(LuaScript::tag<MyStringAnsi>)
{
	const char * str = lua_tostring(this->state, this->stackPtr++);
	return str;
}


//=============================================================================
//===================== Get data returned from LUA (stack at -1) ==============
//=============================================================================

template <typename T>
T LuaScript::GetReturnValue()
{
	void * data = lua_touserdata(this->state, -1);
	return static_cast<T>(data);
};


template <>
inline int LuaScript::GetReturnValue()
{
	return static_cast<int>(lua_tointeger(this->state, 1));
};


template <>
LUA_INLINE MyStringAnsi LuaScript::GetReturnValue()
{
	const char * str = lua_tostring(this->state, 1);
	return str;
}


template <>
LUA_INLINE uint32 LuaScript::GetReturnValue()
{
	return lua_tounsigned(this->state, 1);
}

template <>
LUA_INLINE bool LuaScript::GetReturnValue()
{
	return (lua_toboolean(this->state, 1) == 1);
}

template <>
LUA_INLINE float LuaScript::GetReturnValue()
{
	return static_cast<float>(lua_tonumber(this->state, -1));
}

template <>
LUA_INLINE double LuaScript::GetReturnValue()
{
	return lua_tonumber(this->state, 1);
}

//=============================================================================
//===================== Push data to LUA stack ================================
//=============================================================================

template <typename T>
LUA_INLINE void LuaScript::AddFnReturnValue(T * val)
{
	this->returnValCount++;	
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
};

LUA_INLINE void LuaScript::AddFnReturnValue(bool val)
{
	this->returnValCount++;
	lua_pushboolean(this->state, val);
};



LUA_INLINE void LuaScript::AddFnReturnValue(float val)
{
	this->returnValCount++;
	lua_pushnumber(this->state, val);
};


LUA_INLINE void LuaScript::AddFnReturnValue(double val)
{
	this->returnValCount++;
	lua_pushnumber(this->state, val);
};


LUA_INLINE void LuaScript::AddFnReturnValue(const char * val)
{
	this->returnValCount++;
	lua_pushstring(this->state, val);
};


LUA_INLINE void LuaScript::AddFnReturnValue(const MyStringAnsi & val)
{
	this->returnValCount++;
	lua_pushstring(this->state, val.GetConstString());
};



//=============================================================================
//===================== Set LUA global variable ===============================
//=============================================================================

template <typename T>
LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, T * val)
{	
	
	T ** udata = (T **)lua_newuserdata(this->state, sizeof(T *));
	*udata = val;
	
	luaL_getmetatable(this->state, "Account");
	
	//put the "pointer to data" into "arguments" table
	lua_pushstring(this->state, "__parent");
	lua_pushlightuserdata(this->state, *udata);
	lua_settable(this->state, -3);


	lua_setmetatable(this->state, -2);

	
	lua_setglobal(this->state, varName.GetConstString());
};


template <typename T>
LUA_INLINE void LuaScript::SetGlobalVarLight(const MyStringAnsi & varName, T * val)
{
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
	lua_setglobal(this->state, varName.GetConstString());
};


LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, bool val)
{
	lua_pushboolean(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, float val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, double val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, const char * val)
{
	lua_pushstring(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

LUA_INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, const MyStringAnsi & val)
{
	lua_pushstring(this->state, val.GetConstString());
	lua_setglobal(this->state, varName.GetConstString());
};

