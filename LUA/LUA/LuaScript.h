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

			
			
			

			LuaScript(lua_State * state, const MyStringAnsi & scriptName, const MyStringAnsi & scriptFileName);
			~LuaScript();

			lua_State * GetState();

			void Reload();
			
			void RegisterFunction(const MyStringAnsi & luaFName, lua_CFunction fn);
			
			template <typename T>
			void RegisterClass(const LuaClassBind<T> & classBind);


			bool IsStackEmpty() const;
			int GetStackSize() const;
			int GetFnReturnValueCount() const;

			void Run();

			void Reset();
				
			void IncStack();

			void PrintStack();
			void PrintStack(const MyStringAnsi & id);

			//=============================================================================

			int GetFnInputTableInt(const MyStringAnsi & varName);
			bool GetFnInputTableBool(const MyStringAnsi & varName);
			MyStringAnsi GetFnInputTableString(const MyStringAnsi & varName);
			float GetFnInputTableFloat(const MyStringAnsi & varName);
			double GetFnInputTableDouble(const MyStringAnsi & varName);
			uint32 GetFnInputTableUnsigned(const MyStringAnsi & varName);


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
			void AddFnReturnValue(const MyStringAnsi & val);

			//=============================================================================
			//===================== Set LUA global variable ===============================
			//=============================================================================

			template <typename T>
			void SetGlobalVarClass(const MyStringAnsi & varName, T * val, const MyStringAnsi & className);

			template <typename T>
			void SetGlobalVarLight(const MyStringAnsi & varName, T * val);

			template <typename T, INTEGRAL_SIGNED(T)>
			LUA_INLINE void SetGlobalVar(const MyStringAnsi & varName, T val)
			{
				lua_pushinteger(this->state, val);
				lua_setglobal(this->state, varName.GetConstString());
			}

			template <typename T, INTEGRAL_UNSIGNED(T)>
			LUA_INLINE void SetGlobalVar(const MyStringAnsi & varName, T val)
			{
				lua_pushunsigned(this->state, val);
				lua_setglobal(this->state, varName.GetConstString());
			}
		
			void SetGlobalVar(const MyStringAnsi & varName, bool val);
			void SetGlobalVar(const MyStringAnsi & varName, float val);
			void SetGlobalVar(const MyStringAnsi & varName, double val);
			void SetGlobalVar(const MyStringAnsi & varName, const char * val);
			void SetGlobalVar(const MyStringAnsi & varName, const MyStringAnsi & val);




			friend class LuaWrapper;
			
			

		private:
			lua_State * state;
			MyStringAnsi scriptName;
			MyStringAnsi scriptFileName;
			int runCount;


			int stackPtr;
			int returnValCount;
			
			template <typename T>
			struct tag
			{
				using type = T;
			};


			//=============================================================================

			std::vector<MyStringAnsi> GetAllGlobals();

			
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
			MyStringAnsi GetFnInputImpl(tag<MyStringAnsi>);

		};


		#include "LuaScript.inl"


	
}



#endif