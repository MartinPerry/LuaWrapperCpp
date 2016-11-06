#ifndef LUA_SCRIPT_H
	#include "LuaScript.h"
#endif

extern "C"
{
#include "./lua_lib/lua.h"
#include "./lua_lib/lualib.h"
#include "./lua_lib/lauxlib.h"
}





//=============================================================================
//===================== Get data passed from Lua callback function ============
//=============================================================================


template <typename T>
LUA_INLINE T * LuaScript::GetFnInputImpl(int i, LuaScript::tag<T *>)
{	
	int argType = lua_type(this->state, i);

	if (argType == LUA_TUSERDATA)
	{
		T * a = (*(T **)(lua_touserdata(this->state, i)));
		return a;
	}
	else if (argType == LUA_TLIGHTUSERDATA)
	{
		return static_cast<T *>(lua_touserdata(this->state, i));
	}
	else
	{
		return NULL;
	}
};


LUA_INLINE bool LuaScript::GetFnInputImpl(int i, LuaScript::tag<bool>)
{
	return (lua_toboolean(this->state, i) == 1);
};


LUA_INLINE float LuaScript::GetFnInputImpl(int i, LuaScript::tag<float>)
{
	return static_cast<float>(lua_tonumber(this->state, i));
};

LUA_INLINE double LuaScript::GetFnInputImpl(int i, LuaScript::tag<double>)
{
	return lua_tonumber(this->state, i);
};


LUA_INLINE LuaString LuaScript::GetFnInputImpl(int i, LuaScript::tag<LuaString>)
{
	const char * str = lua_tostring(this->state, i);
	return str;
};



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


LUA_INLINE void LuaScript::AddFnReturnValue(const LuaString & val)
{
	this->returnValCount++;
	lua_pushstring(this->state, val.c_str());
};



//=============================================================================
//===================== Set LUA global variable ===============================
//=============================================================================


template <typename T>
LUA_INLINE void LuaScript::SetGlobalVarClass(const LuaString & varName, T * val)
{	
	
	LuaCallbacks::SetNewUserDataClass<T>(this->state, val);
	
	lua_setglobal(this->state, varName.c_str());
};


template <typename T>
LUA_INLINE void LuaScript::SetGlobalVarLight(const LuaString & varName, T * val)
{
	lua_pushlightuserdata(this->state, static_cast<void *>(val));
	lua_setglobal(this->state, varName.c_str());
};



