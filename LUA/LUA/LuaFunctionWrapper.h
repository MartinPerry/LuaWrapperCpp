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

#include "./LuaWrapper.h"
#include "./LuaClassBind.h"

#include "./LuaMacros.h"
#include "./LuaTypes.h"

namespace Lua 
{

	/*-----------------------------------------------------------
	Class:	LuaFunctionsWrapper
	
	Contains static methods as wrapper around Lua calls
	This is because of template use and choose the right call
	-------------------------------------------------------------*/
	class LuaFunctionsWrapper
	{
		public:

			template <class T>
			static LUA_INLINE auto GetFnInput(lua_State * L, int i) -> decltype(GetFnInputImpl(L, i, tag<T>{}))
			{
				return LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T>{});
			};


			
			
		
		private:
			template <typename T>
			struct tag
			{
				using type = T;
			};
		


			

			//=============================================================================
			//===================== Get data passed from Lua callback function ================================
			//=============================================================================


			template <typename T>
			static LUA_INLINE T * GetFnInputImpl(lua_State * L, int i, tag<T *>);

			template <typename T>
			static LUA_INLINE T & GetFnInputImpl(lua_State * L, int i, tag<T &>)
			{
				return *(LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T *>{}));
			};


			template <typename T, INTEGRAL_SIGNED(T)>
			static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>)
			{
				return static_cast<T>(lua_tointeger(L, i));
			};

			template <typename T, INTEGRAL_UNSIGNED(T)>
			static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>)
			{
				return static_cast<T>(lua_tounsigned(L, i));
			};


			static double GetFnInputImpl(lua_State * L, int i, tag<double>);
			static float GetFnInputImpl(lua_State * L, int i, tag<float>);
			static bool GetFnInputImpl(lua_State * L, int i, tag<bool>);
			static LuaString GetFnInputImpl(lua_State * L, int i, tag<LuaString>);

	};

	#include "LuaFunctionWrapper.inl"
}

#endif
