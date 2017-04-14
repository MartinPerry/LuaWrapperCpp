#ifndef LUA_SCRIPT_H
	#include "LuaScript.h"
#endif



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




LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, bool val)
{
	lua_pushboolean(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, float val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, double val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, const char * val)
{
	lua_pushstring(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, const LuaString & val)
{
	lua_pushstring(this->state, val.c_str());
	lua_setglobal(this->state, varName.c_str());
};

