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


LUA_INLINE LuaString LuaScript::GetFnInputImpl(LuaScript::tag<LuaString>)
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



