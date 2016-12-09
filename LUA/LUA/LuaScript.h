#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H


#include <typeinfo>
#include <typeindex>
#include <type_traits>

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
		
		LuaScript(lua_State * state, const LuaString & scriptName, const LuaString & scriptFileName);
		~LuaScript();

		lua_State * GetState();

		
		void RegisterFunction(const LuaString & luaFName, lua_CFunction fn);


		void RegisterClass(const LuaClass & classBind);
		

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
		void Push(T * val);


		template <typename T,
			typename std::enable_if <
			std::is_integral<T>::value == false &&
			std::is_same<T, float>::value == false &&
			std::is_same<T, double>::value == false &&
			std::is_same<T, const char *>::value == false &&
			std::is_same<T, const LuaString &>::value == false
			>::type* = nullptr
		>			
		void Push(T val)
		{
			this->returnValCount++;			
			T * v = reinterpret_cast<T*>(::operator new(sizeof(T)));
			memcpy(v, &val, sizeof(T));
			LuaCallbacks::SetNewUserDataClass<T>(this->state, v);
		}

		template <typename T, INTEGRAL_SIGNED(T)>
		LUA_INLINE void Push(T val)
		{
			this->returnValCount++;
			lua_pushinteger(this->state, val);
		};

		template <typename T, INTEGRAL_UNSIGNED(T)>
		LUA_INLINE void Push(T val)
		{
			this->returnValCount++;
			lua_pushunsigned(this->state, val);
		};

		void Push(bool val);
		void Push(float val);
		void Push(double val);
		void Push(const char * val);
		void Push(const LuaString & val);

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
			lua_setglobal(this->state, varName.GetConstString());
		}

		template <typename T, INTEGRAL_UNSIGNED(T)>
		LUA_INLINE void SetGlobalVar(const LuaString & varName, T val)
		{
			lua_pushunsigned(this->state, val);
			lua_setglobal(this->state, varName.GetConstString());
		}

		void SetGlobalVar(const LuaString & varName, bool val);
		void SetGlobalVar(const LuaString & varName, float val);
		void SetGlobalVar(const LuaString & varName, double val);
		void SetGlobalVar(const LuaString & varName, const char * val);
		void SetGlobalVar(const LuaString & varName, const LuaString & val);




		friend class LuaWrapper;



	private:
		lua_State * state;
		LuaString scriptName;
		LuaString scriptFileName;
		int runCount;
		bool returnLightUserData;
			
		int returnValCount;




		//=============================================================================

		std::vector<LuaString> GetAllGlobals();

		



	};


#include "LuaScript.inl"



};



#endif