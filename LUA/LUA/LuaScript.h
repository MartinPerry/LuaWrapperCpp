#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H



#include <vector>


#include "../Macros.h"
#include "../Strings/MyStringAnsi.h"


#include "./LuaWrapper.h"
#include "./LuaClassBind.h"
#include "./LuaMacros.h"



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

			void Reload();
			
			void RegisterFunction(const LuaString & luaFName, lua_CFunction fn);
			
			
			void RegisterClass(const LuaClass & classBind);
			void RegisterClass(const LuaClass * classBind);


			bool IsStackEmpty() const;
			int GetStackSize() const;
			int GetFnReturnValueCount() const;

			void Run();

			void Reset();
				
			void IncStack();

			void PrintStack();
			void PrintStack(const LuaString & id);

			//=============================================================================


			//=============================================================================
			//===================== Get data passed from Lua callback function ================================
			//=============================================================================

			template <class T>
			LUA_INLINE auto GetFnInput() -> decltype(GetFnInputImpl(tag<T>{}))
			{
				return GetFnInputImpl(tag<T>{});
			};

			//=============================================================================
			//===================== Get data returned from LUA (stack at -1) ================================
			//=============================================================================


			template <typename T>
			T GetReturnValue();
			
			//=============================================================================
			//===================== Push data to LUA stack ================================
			//=============================================================================

			template <typename T>
			void AddFnReturnValue(T * val);

			template <typename T, INTEGRAL_SIGNED(T)>
			LUA_INLINE void AddFnReturnValue(T val)
			{
				this->returnValCount++;
				lua_pushinteger(this->state, val);
			};

			template <typename T, INTEGRAL_UNSIGNED(T)>
			LUA_INLINE void AddFnReturnValue(T val)
			{
				this->returnValCount++;
				lua_pushunsigned(this->state, val);
			};

			void AddFnReturnValue(bool val);
			void AddFnReturnValue(float val);
			void AddFnReturnValue(double val);
			void AddFnReturnValue(const char * val);
			void AddFnReturnValue(const LuaString & val);

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

			int stackPtr;
			int returnValCount;
			
			template <typename T>
			struct tag
			{
				using type = T;
			};


			//=============================================================================

			std::vector<LuaString> GetAllGlobals();

			
			//=============================================================================
			//===================== Get data passed from Lua callback function ================================
			//=============================================================================

			
			template <typename T>
			T * GetFnInputImpl(tag<T *>);

			template <typename T>
			LUA_INLINE T & GetFnInputImpl(tag<T &>)
			{
				return *(this->GetFnInputImpl(LuaScript::tag<T *>{}));
			};


			template <typename T, INTEGRAL_SIGNED(T)>
			LUA_INLINE T GetFnInputImpl(tag<T>)
			{
				return static_cast<T>(lua_tointeger(this->state, this->stackPtr++));
			};

			template <typename T, INTEGRAL_UNSIGNED(T)>
			LUA_INLINE T GetFnInputImpl(tag<T>)
			{
				return static_cast<T>(lua_tounsigned(this->state, this->stackPtr++));
			};

							
			double GetFnInputImpl(tag<double>);
			float GetFnInputImpl(tag<float>);
			bool GetFnInputImpl(tag<bool>);
			LuaString GetFnInputImpl(tag<LuaString>);

		};


		#include "LuaScript.inl"


	
}



#endif