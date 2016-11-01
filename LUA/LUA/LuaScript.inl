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

	//this->PrintStack("METATABLE");
	
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

	MyStringAnsi argsMetatable = classBind.className;
	argsMetatable += "_atrs";
	luaL_newmetatable(L, argsMetatable.c_str()); //mt1
	int tableID = lua_gettop(L);
					 // push a temporary variable into the imt
		
	lua_pushliteral(L, "__index");
#if (SAFE_PTR_CHECKS == 1)	
	lua_pushstring(L, argsMetatable.c_str());
	lua_pushcclosure(L, LuaCallbacks::index<T>, 1);
#else
	lua_pushcclosure(L, LuaCallbacks::index<T>, 0);
#endif	
	lua_settable(L, tableID);

	
	// set mt1.__index = mt1
	//lua_pushstring(L, "__index");
	//lua_pushvalue(L, -2);
	//lua_settable(L, -3);
	

	// set mt1.__newindex = mt1
	lua_pushstring(L, "__newindex");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_setmetatable(L, -2);	// setmetatable(mt2, mt1)


	for (size_t i = 0; i < classBind.attrs.size() - 1; i++)
	{
		//lua_pushlightuserdata(L, classBind.attrs[i].func);
		//lua_setfield(L, -2, classBind.attrs[i].name);

		lua_pushstring(L, classBind.attrs[i].name);
		lua_pushlightuserdata(L, classBind.attrs[i].func);
		lua_settable(L, -3);
	}

	
	//this->PrintStack("setmeta");
	
			
			
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

	LuaCallbacks::SetNewUserDataClass(this->state, val, "Account");
	//lua_pushlightuserdata(this->state, static_cast<void *>(val));
	this->PrintStack("xxxx");
	
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
LUA_INLINE void LuaScript::SetGlobalVarClass(const MyStringAnsi & varName, T * val, 
	const MyStringAnsi & className = typeid(T).name())
{	
	
	LuaCallbacks::SetNewUserDataClass(this->state, val, className);
	
	lua_setglobal(this->state, varName.GetConstString());
};


template <typename T>
LUA_INLINE void LuaScript::SetGlobalVarLight(const MyStringAnsi & varName, T * val)
{
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
	lua_setglobal(this->state, varName.GetConstString());
};



