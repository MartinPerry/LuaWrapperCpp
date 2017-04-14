#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H


#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <memory>

#include <vector>

extern "C"
{
#include "./lua_lib/lua.h"
#include "./lua_lib/lualib.h"
#include "./lua_lib/lauxlib.h"
}

#include "./LuaWrapper.h"
#include "./LuaClassBind.h"

#include "./LuaMacros.h"
#include "./LuaTypes.h"




#define INTEGRAL_SIGNED(T) typename std::enable_if <std::is_integral<T>::value>::type* = nullptr, \
		typename std::enable_if <std::is_signed<T>::value == true>::type* = nullptr

#define INTEGRAL_UNSIGNED(T) typename std::enable_if <std::is_integral<T>::value>::type* = nullptr, \
		typename std::enable_if <std::is_signed<T>::value == false>::type* = nullptr	


namespace Lua
{
	class LuaScript
	{
	public:
		
		LuaScript(lua_State * state, const LuaString & scriptName);
		~LuaScript();

		lua_State * GetState();

		
		void RegisterLuaFunction(const LuaString & luaFName, lua_CFunction fn);


		void RegisterLuaClass(const LuaClass & classBind);
		

		bool IsStackEmpty() const;
		int GetStackSize() const;
		int GetFnReturnValueCount() const;

		void Run();
		
		void Reset();
		
		void PrintStack();
		void PrintStack(const LuaString & id);

		//=============================================================================




		//=============================================================================
		//===================== Push data to LUA stack ================================
		//=============================================================================

		template <typename T>
		void Push(T * val)
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

		
		template <typename T>
		void Push(T val)
		{
			this->returnValCount++;
			LuaFunctionsWrapper::Push(this->state, val);
		};
		
		//=============================================================================
		//===================== Set LUA global variable ===============================
		//=============================================================================

		template <typename T>
		void SetGlobalVarClass(const LuaString & varName, T * val);

		template <typename T>
		void SetGlobalVarLight(const LuaString & varName, T * val);

		template <typename T, INTEGRAL_SIGNED(T)>
		LUA_INLINE void SetGlobalVar(const LuaString & varName, T val)
		{
			lua_pushinteger(this->state, val);
			lua_setglobal(this->state, varName.c_str());
		}

		template <typename T, INTEGRAL_UNSIGNED(T)>
		LUA_INLINE void SetGlobalVar(const LuaString & varName, T val)
		{
			lua_pushunsigned(this->state, val);
			lua_setglobal(this->state, varName.c_str());
		}

		void SetGlobalVar(const LuaString & varName, bool val);
		void SetGlobalVar(const LuaString & varName, float val);
		void SetGlobalVar(const LuaString & varName, double val);
		void SetGlobalVar(const LuaString & varName, const char * val);
		void SetGlobalVar(const LuaString & varName, const LuaString & val);




		friend class LuaWrapper;
		friend class LuaFunction;


	private:
		lua_State * state;
		LuaString scriptName;
		
		int runCount;
		bool returnLightUserData;
			
		int returnValCount;




		//=============================================================================

		std::vector<LuaString> GetAllGlobals();

		



	};


#include "LuaScript.inl"



};



#endif