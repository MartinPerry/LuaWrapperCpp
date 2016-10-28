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

	lua_State * L = this->state;

	
	// Create a luaL metatable. This metatable is not 
	// exposed to Lua. The "classBind.className.c_str()" label is used by luaL
	// internally to identity things.
	luaL_newmetatable(L, classBind.className.c_str());
	int metaTableID = lua_gettop(L);

	// Register the C functions _into_ the metatable we just created.
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classBind.className.c_str());
	luaL_setfuncs(L, &classBind.methods[0], 1);
#else
	luaL_setfuncs(L, &classBind.methods[0], 0);
#endif

	// The Lua stack at this point looks like this:
	//     
	//     1| metatable "classBind.className.c_str()"   |-1
	lua_pushvalue(L, -1);

	// The Lua stack at this point looks like this:
	//     
	//     2| metatable "classBind.className.c_str()"   |-1
	//     1| metatable "classBind.className.c_str()"   |-2

	// Set the "__index" field of the metatable to point to itself
	// This pops the stack
	lua_setfield(L, -1, "__index");

	// The Lua stack at this point looks like this:
	//     
	//     1| metatable "classBind.className.c_str()"   |-1

	// The classBind.className.c_str() metatable now has the following fields
	//     - __gc
	//     - __index
	//     - .. methods..


	lua_pushstring(L, classBind.className.c_str());	
	lua_pushcclosure(L, LuaCallbacks::create_new<T>, 1);
	lua_setglobal(L, classBind.className.c_str()); // this is how function will be named in Lua

	
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

	
}



//=============================================================================
//===================== Get data passed from Lua callback function ============
//=============================================================================


//template<>
//struct LuaScript::item_return<const MyStringAnsi &> { typedef MyStringAnsi type; };


template <typename T>
INLINE T * LuaScript::GetFnInputImpl(LuaScript::tag<T *>)
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


INLINE bool LuaScript::GetFnInputImpl(LuaScript::tag<bool>)
{
	return (lua_toboolean(this->state, this->stackPtr++) == 1);
}


INLINE float LuaScript::GetFnInputImpl(LuaScript::tag<float>)
{
	return static_cast<float>(lua_tonumber(this->state, this->stackPtr++));
}

INLINE double LuaScript::GetFnInputImpl(LuaScript::tag<double>)
{
	return lua_tonumber(this->state, this->stackPtr++);
}


INLINE MyStringAnsi LuaScript::GetFnInputImpl(LuaScript::tag<MyStringAnsi>)
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
INLINE MyStringAnsi LuaScript::GetReturnValue()
{
	const char * str = lua_tostring(this->state, 1);
	return str;
}


template <>
INLINE uint32 LuaScript::GetReturnValue()
{
	return lua_tounsigned(this->state, 1);
}

template <>
INLINE bool LuaScript::GetReturnValue()
{
	return (lua_toboolean(this->state, 1) == 1);
}

template <>
INLINE float LuaScript::GetReturnValue()
{
	return static_cast<float>(lua_tonumber(this->state, -1));
}

template <>
INLINE double LuaScript::GetReturnValue()
{
	return lua_tonumber(this->state, 1);
}

//=============================================================================
//===================== Push data to LUA stack ================================
//=============================================================================

template <typename T>
INLINE void LuaScript::AddFnReturnValue(T * val)
{
	this->returnValCount++;	
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
};

INLINE void LuaScript::AddFnReturnValue(bool val)
{
	this->returnValCount++;
	lua_pushboolean(this->state, val);
};



INLINE void LuaScript::AddFnReturnValue(float val)
{
	this->returnValCount++;
	lua_pushnumber(this->state, val);
};


INLINE void LuaScript::AddFnReturnValue(double val)
{
	this->returnValCount++;
	lua_pushnumber(this->state, val);
};


INLINE void LuaScript::AddFnReturnValue(const char * val)
{
	this->returnValCount++;
	lua_pushstring(this->state, val);
};


INLINE void LuaScript::AddFnReturnValue(const MyStringAnsi & val)
{
	this->returnValCount++;
	lua_pushstring(this->state, val.GetConstString());
};



//=============================================================================
//===================== Set LUA global variable ===============================
//=============================================================================

template <typename T>
INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, T * val)
{	
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
	lua_setglobal(this->state, varName.GetConstString());
};



INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, bool val)
{
	lua_pushboolean(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, float val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, double val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, const char * val)
{
	lua_pushstring(this->state, val);
	lua_setglobal(this->state, varName.GetConstString());
};

INLINE void LuaScript::SetGlobalVar(const MyStringAnsi & varName, const MyStringAnsi & val)
{
	lua_pushstring(this->state, val.GetConstString());
	lua_setglobal(this->state, varName.GetConstString());
};

