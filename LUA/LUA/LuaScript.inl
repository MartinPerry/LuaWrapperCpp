#ifndef LUA_SCRIPT_H
	#include "LuaScript.h"
#endif

extern "C"
{
	#include "./lua_lib/lua.h"
	#include "./lua_lib/lualib.h"
	#include "./lua_lib/lauxlib.h"
}

#include <typeinfo>
#include <typeindex>

/*-----------------------------------------------------------
Function:	Register
Parameters:

Returns:


Register new lua binding for class
-------------------------------------------------------------*/
template <typename T>
void LuaScript::RegisterClass(const LuaClassBind<T> & classBind)
{
	std::type_index key = std::type_index(typeid(T));

	if (LuaCallbacks::ctors.find(key) != LuaCallbacks::ctors.end())
	{
		MY_LOG_ERROR("Class %s already registered - std::type_index already exist", 
			classBind.ctorName.c_str());
		return;
	}

	const char * classTableName = classBind.ctorName.c_str();

	MyStringAnsi argsMetatable = classTableName;
	argsMetatable += "_attrs";
	const char * classArgsTableName = argsMetatable.c_str();

	LuaCallbacks::ctors[key] = classBind.ctor;
	LuaCallbacks::toString[key] = classBind.toString;
	LuaCallbacks::tableName[key] = classTableName;

	this->returnLightUserData = classBind.returnLightUserData;

	//http://cfc.kizzx2.com/index.php/binding-c-classes-to-lua-a-step-by-step-example-for-beginners/
	//http://lua-users.org/lists/lua-l/2006-08/msg00245.html


	lua_State * L = this->state;

		
	luaL_newmetatable(L, classTableName); //mt2
	int metaTableID = lua_gettop(L);


	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");

	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__newindex");

	/*
	lua_pushliteral(L, "__index");
	lua_pushstring(L, classTableName);
	lua_pushcclosure(L, LuaCallbacks::index<T>, 1);
	lua_rawset(L, metaTableID);


	lua_pushliteral(L, "__newindex");
	lua_pushstring(L, classTableName);
	lua_pushcclosure(L, LuaCallbacks::new_index<T>, 1);
	lua_rawset(L, metaTableID);
	*/

	
	//lua_pushstring(L, classBind.className.c_str());	
	lua_pushcclosure(L, LuaCallbacks::create_new<T>, 0);
	lua_setglobal(L, classBind.ctorName.c_str()); // this is how function will be named in Lua


	// Register the C functions _into_ the metatable we just created.	
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classTableName);
	luaL_setfuncs(L, &classBind.methods[0], 1);
#else
	luaL_setfuncs(L, &classBind.methods[0], 0);
#endif
					
	//double underscore variables
	//http://nova-fusion.com/2011/06/30/lua-metatables-tutorial/
	//http://stackoverflow.com/questions/10891957/difference-between-tables-and-metatables-in-lua


	//rewrite __gc with our own
	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, LuaCallbacks::garbage_collect<T>);	
	lua_rawset(L, metaTableID);

	

	lua_pushliteral(L, "__tostring");
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classTableName);
	lua_pushcclosure(L, LuaCallbacks::to_string<T>, 1);
#else
	lua_pushcclosure(L, LuaCallbacks::to_string<T>, 0);
#endif
	lua_rawset(L, metaTableID);

			
	//return;
	//---------------------------------------------------------------------

	lua_pop(L, -1);
	luaL_getmetatable(L, classTableName);
	
	luaL_newmetatable(L, classArgsTableName); //mt1
	int tableID = lua_gettop(L);
					 
	
	lua_pushliteral(L, "__index");
	lua_pushstring(L, classArgsTableName);
	lua_pushcclosure(L, LuaCallbacks::index<T>, 1);
	lua_rawset(L, tableID);


	lua_pushliteral(L, "__newindex");
	lua_pushstring(L, classArgsTableName);
	lua_pushcclosure(L, LuaCallbacks::new_index<T>, 1);
	lua_rawset(L, tableID);
	
	// set mt1.__index = mt1
	//lua_pushstring(L, "__index");
	//lua_pushvalue(L, -2);
	//lua_settable(L, -3);
	

	// set mt1.__newindex = mt1
	//lua_pushstring(L, "__newindex");
	//lua_pushvalue(L, -2);
	//lua_settable(L, -3);


	for (size_t i = 0; i < classBind.attrs.size() - 1; i++)
	{		
		lua_pushstring(L, classBind.attrs[i].name);
		lua_pushlightuserdata(L, classBind.attrs[i].func);
		lua_rawset(L, -3);
	}
	
	lua_setmetatable(L, -2);	// setmetatable(mt2, mt1)					
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
	if (this->returnLightUserData == false)
	{
		LuaCallbacks::SetNewUserDataClass(this->state, val);
	}
	else
	{
		lua_pushlightuserdata(this->state, static_cast<void *>(val));
	}
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
LUA_INLINE void LuaScript::SetGlobalVarClass(const MyStringAnsi & varName, T * val)
{	
	
	LuaCallbacks::SetNewUserDataClass<T>(this->state, val);
	
	lua_setglobal(this->state, varName.GetConstString());
};


template <typename T>
LUA_INLINE void LuaScript::SetGlobalVarLight(const MyStringAnsi & varName, T * val)
{
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
	lua_setglobal(this->state, varName.GetConstString());
};



