
//=============================================================================
//===================== Get data passed from Lua callback function ============
//=============================================================================


template <typename T>
LUA_INLINE T * LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, LuaFunctionsWrapper::tag<T *>)
{
	int argType = lua_type(L, i);

	if (argType == LUA_TUSERDATA)
	{
		T * a = (*(T **)(lua_touserdata(L, i)));
		return a;
	}
	else if (argType == LUA_TLIGHTUSERDATA)
	{
		return static_cast<T *>(lua_touserdata(L, i));
	}
	else
	{
		return NULL;
	}
};


LUA_INLINE bool LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, LuaFunctionsWrapper::tag<bool>)
{
	return (lua_toboolean(L, i) == 1);
};


LUA_INLINE float LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, LuaFunctionsWrapper::tag<float>)
{
	return static_cast<float>(lua_tonumber(L, i));
};

LUA_INLINE double LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, LuaFunctionsWrapper::tag<double>)
{
	return lua_tonumber(L, i);
};


LUA_INLINE LuaString LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, LuaFunctionsWrapper::tag<LuaString>)
{
	const char * str = lua_tostring(L, i);
	return str;
};

