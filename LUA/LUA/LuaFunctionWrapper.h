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
		
			void Test()
			{
			}

			template <class T>
			static LUA_INLINE T GetFnInput(lua_State * L, int i)
				//-> decltype(GetFnInputImpl(L, i, tag<T>{}))
			{
				return LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T>{});
			};


			
			template <class T>
			static LUA_INLINE void Push(lua_State * L, T v)				
			{
				LuaFunctionsWrapper::PushImpl(L, v, tag<T>{});
			};
			
			
		
		private:

			//=============================================================================
			//===================== Get data passed from Lua callback function ================================
			//=============================================================================


			template <typename T>
			static LUA_INLINE T * GetFnInputImpl(lua_State * L, int i, tag<T *>)
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
			static LUA_INLINE T & GetFnInputImpl(lua_State * L, int i, tag<T &>)
			{
				return *(LuaFunctionsWrapper::GetFnInputImpl(L, i, tag<T *>{}));
			};


			template <typename T, 
				typename std::enable_if <
					std::is_integral<T>::value == false &&
					std::is_same<T, float>::value == false &&
					std::is_same<T, double>::value == false &&					
					std::is_same<T, LuaString>::value == false
				>::type* = nullptr
			>
			static LUA_INLINE T GetFnInputImpl(lua_State * L, int i, tag<T>)
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


			static LUA_INLINE double GetFnInputImpl(lua_State * L, int i, tag<double>)
			{
				return lua_tonumber(L, i);
			};

			static LUA_INLINE float GetFnInputImpl(lua_State * L, int i, tag<float>)
			{
				return static_cast<float>(lua_tonumber(L, i));
			};

			static LUA_INLINE bool GetFnInputImpl(lua_State * L, int i, tag<bool>)
			{
				return (lua_toboolean(L, i) == 1);
			};

			static LUA_INLINE LuaString GetFnInputImpl(lua_State * L, int i, tag<LuaString>)
			{
				const char * str = lua_tostring(L, i);
				return str;
			};


			//========================================================================
			// Push to Lua
			//========================================================================

			
			template <typename T>
			static LUA_INLINE void PushImpl(lua_State * L, T * val, tag<T *>)
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
			static LUA_INLINE void PushImpl(lua_State * L, T & val, tag<T &>)
			{
				LuaFunctionsWrapper::PushImpl(L, &val, tag<T *>{});
			};


			template <typename T,
				typename std::enable_if <
				std::is_integral<T>::value == false &&
				std::is_same<T, float>::value == false &&
				std::is_same<T, double>::value == false &&
				std::is_same<T, const char *>::value == false &&
				std::is_same<T, const LuaString &>::value == false
				>::type* = nullptr
			>
				static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>)
			{
				T * v = reinterpret_cast<T*>(::operator new(sizeof(T)));
				memcpy(v, &val, sizeof(T));
				LuaCallbacks::SetNewUserDataClass<T>(L, v);
			};

			template <typename T, INTEGRAL_SIGNED(T)>
			static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>)
			{				
				lua_pushinteger(L, val);
			};

			template <typename T, INTEGRAL_UNSIGNED(T)>
			static LUA_INLINE void PushImpl(lua_State * L, T val, tag<T>)
			{			
				lua_pushunsigned(L, val);
			};


			static LUA_INLINE void PushImpl(lua_State * L, double val, tag<double>)
			{
				lua_pushnumber(L, val);
			};

			static LUA_INLINE void PushImpl(lua_State * L, float val, tag<float>)
			{
				lua_pushnumber(L, val);
			};

			static LUA_INLINE void PushImpl(lua_State * L, bool val, tag<bool>)
			{
				lua_pushboolean(L, val);
			};

			static LUA_INLINE void PushImpl(lua_State * L, const LuaString & val, tag<const LuaString &>)
			{
				lua_pushstring(L, val.c_str());
			};

			static LUA_INLINE void PushImpl(lua_State * L, const char * val, tag<const char *>)
			{
				lua_pushstring(L, val);
			};
						
	};	
}

#endif
