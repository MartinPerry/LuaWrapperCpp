#ifndef LUA_FUNCTIONS_WRAPPER
#define LUA_FUNCTIONS_WRAPPER


extern "C"
{
#include "./lua_lib/lua.h"
#include "./lua_lib/lualib.h"
#include "./lua_lib/lauxlib.h"
}

#include <typeinfo>
#include <typeindex>
#include <type_traits>

#include <vector>


#include "./LuaMacros.h"
#include "./LuaTypes.h"


#include "./LuaClassBind.h"




namespace Lua
{

	/*-----------------------------------------------------------
	Class:	LuaFunctionsWrapper

	Contains static methods as wrapper around Lua calls
	This is because of template use and choose the right call
	-------------------------------------------------------------*/
	class LuaFunctionsWrapper
	{
	private:

		template <typename T>
		struct tag { using type = T; };

	public:

		template <class T>
		static LUA_INLINE T GetFnInput(lua_State * L, int i);
		//-> decltype(GetFnInputImpl(L, i, tag<T>{}))
	//{
	//	return LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T>{});
	//};



		template <class T>
		static LUA_INLINE void Push(lua_State * L, T v);




	private:

		//=============================================================================
		//=================== Get data passed from Lua callback function ==============
		//=============================================================================
		
		
		template <typename T>
		static LUA_INLINE T * GetFnInputImpl(lua_State * L, int i, tag<T *>);


		template <typename T>
		static LUA_INLINE T & GetFnInputImpl(lua_State * L, int i, tag<T &>);


		template <typename T,
			typename = typename std::enable_if 
			<
				std::is_integral<T>::value == false &&
				std::is_same<T, float>::value == false &&
				std::is_same<T, double>::value == false &&
				std::is_same<T, LuaString>::value == false
			>::type
		>
		static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>);

		template <typename T, INTEGRAL_SIGNED(T)>
		static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>);

		template <typename T, INTEGRAL_UNSIGNED(T)>
		static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>);

		static LUA_INLINE double GetFnInputImpl(lua_State * L, int i, tag<double>);
		static LUA_INLINE float GetFnInputImpl(lua_State * L, int i, tag<float>);
		static LUA_INLINE bool GetFnInputImpl(lua_State * L, int i, tag<bool>);
		static LUA_INLINE LuaString GetFnInputImpl(lua_State * L, int i, tag<LuaString>);


		//========================================================================
		// Push to Lua
		//========================================================================


		template <typename T>
		static LUA_INLINE void PushImpl(lua_State * L, T * val, tag<T *>);

		template <typename T>
		static LUA_INLINE void PushImpl(lua_State * L, T & val, tag<T &>);


		template <typename T,
			typename = typename std::enable_if 
			<
				std::is_integral<T>::value == false &&
				std::is_same<T, float>::value == false &&
				std::is_same<T, double>::value == false &&
				std::is_same<T, const char *>::value == false &&
				std::is_same<T, const LuaString &>::value == false
			>::type
		>
		static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>);

		template <typename T, INTEGRAL_SIGNED(T)>
		static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>);

		template <typename T, INTEGRAL_UNSIGNED(T)>
		static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>);


		static LUA_INLINE void PushImpl(lua_State * L, double val, tag<double>);
		static LUA_INLINE void PushImpl(lua_State * L, float val, tag<float>);
		static LUA_INLINE void PushImpl(lua_State * L, bool val, tag<bool>);
		static LUA_INLINE void PushImpl(lua_State * L, const LuaString & val, tag<const LuaString &>);
		static LUA_INLINE void PushImpl(lua_State * L, const char * val, tag<const char *>);

	};

	//=============================================================================================
	// Implementation
	//=============================================================================================

	
	template <class T>
	LUA_INLINE T LuaFunctionsWrapper::GetFnInput(lua_State * L, int i)
		//-> decltype(GetFnInputImpl(L, i, tag<T>{}))
	{
		return LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T>{});
	};



	template <class T>
	LUA_INLINE void LuaFunctionsWrapper::Push(lua_State * L, T v)
	{
		LuaFunctionsWrapper::PushImpl(L, v, tag<T>{});
	};



	template <typename T>
	LUA_INLINE T * LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<T *>)
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

	template <typename T>
	LUA_INLINE T & LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<T &>)
	{
		return *(LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T *>{}));
	};


	template <typename T,
		typename
	>
		LUA_INLINE T LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<T>)
	{
		return *(LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T *>{}));
	};

	template <typename T, INTEGRAL_SIGNED_IMPL(T)>
	LUA_INLINE T LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<T>)
	{
		return static_cast<T>(lua_tointeger(L, i));
	};

	template <typename T, INTEGRAL_UNSIGNED_IMPL(T)>
	LUA_INLINE T LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<T>)
	{
		return static_cast<T>(lua_tounsigned(L, i));
	};


	LUA_INLINE double LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<double>)
	{
		return lua_tonumber(L, i);
	};

	LUA_INLINE float LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<float>)
	{
		return static_cast<float>(lua_tonumber(L, i));
	};

	LUA_INLINE bool LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<bool>)
	{
		return (lua_toboolean(L, i) == 1);
	};

	LUA_INLINE LuaString LuaFunctionsWrapper::GetFnInputImpl(lua_State * L, int i, tag<LuaString>)
	{
		const char * str = lua_tostring(L, i);
		return str;
	};


	//========================================================================
	// Push to Lua
	//========================================================================


	template <typename T>
	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, T * val, tag<T *>)
	{
		lua_pushlightuserdata(L, val);


		//int argType = lua_type(L, i);

		//if (argType == LUA_TUSERDATA)
		//{
		//	T * a = (*(T **)(lua_touserdata(L, i)));
		//	return a;
		//}
		//else if (argType == LUA_TLIGHTUSERDATA)
		//{
		//	return static_cast<T *>(lua_touserdata(L, i));
		//}				

	};

	template <typename T>
	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, T & val, tag<T &>)
	{
		LuaFunctionsWrapper::PushImpl(L, &val, tag<T *>{});
	};


	template <typename T,
		typename
	>
		LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, T val, tag<T>)
	{
		T * v = reinterpret_cast<T*>(::operator new(sizeof(T)));
		memcpy(v, &val, sizeof(T));
		LuaCallbacks::SetNewUserDataClass<T>(L, v);
	};

	template <typename T, INTEGRAL_SIGNED_IMPL(T)>
	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, T val, tag<T>)
	{
		lua_pushinteger(L, val);
	};

	template <typename T, INTEGRAL_UNSIGNED_IMPL(T)>
	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, T val, tag<T>)
	{
		lua_pushunsigned(L, val);
	};


	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, double val, tag<double>)
	{
		lua_pushnumber(L, val);
	};

	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, float val, tag<float>)
	{
		lua_pushnumber(L, val);
	};

	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, bool val, tag<bool>)
	{
		lua_pushboolean(L, val);
	};

	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, const LuaString & val, tag<const LuaString &>)
	{
		lua_pushstring(L, val.c_str());
	};

	LUA_INLINE void LuaFunctionsWrapper::PushImpl(lua_State * L, const char * val, tag<const char *>)
	{
		lua_pushstring(L, val);
	};



}

#endif
