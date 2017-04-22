#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

namespace Lua
{
	struct LuaClass;
}

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

#include "./LuaMacros.h"
#include "./LuaTypes.h"


#include "./LuaFunctionWrapper.h"

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
		void Push(T * val);
		

		
		template <typename T>
		void Push(T val);
		
		//=============================================================================
		//===================== Set LUA global variable ===============================
		//=============================================================================

		template <typename T>
		void SetGlobalVarClass(const LuaString & varName, T * val);

		template <typename T>
		void SetGlobalVarLight(const LuaString & varName, T * val);



		template <typename T, INTEGRAL_SIGNED(T)>		
		void SetGlobalVar(const LuaString & varName, T val);
		
		template <typename T, INTEGRAL_UNSIGNED(T)>		
		void SetGlobalVar(const LuaString & varName, T val);
		

		void SetGlobalVar(const LuaString & varName, bool val);
		void SetGlobalVar(const LuaString & varName, float val);
		void SetGlobalVar(const LuaString & varName, double val);
		void SetGlobalVar(const LuaString & varName, const char * val);
		void SetGlobalVar(const LuaString & varName, const LuaString & val);




		//friend class LuaWrapper;
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
