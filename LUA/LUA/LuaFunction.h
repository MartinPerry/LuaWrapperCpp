#ifndef LUA_FUNCTION_H
#define LUA_FUNCTION_H

#include <iostream>
#include <memory>

extern "C"
{
#include "./lua_lib/lua.h"
#include "./lua_lib/lualib.h"
#include "./lua_lib/lauxlib.h"
}


#include "./LuaMacros.h"
#include "./LuaTypes.h"

#include "./LuaScript.h"
#include "./LuaFunctionWrapper.h"

//https://www.lua.org/pil/25.2.html

namespace Lua
{
	
	class LuaFunction
	{
	public:

		template <typename T>
		using ReturnTypeVoid = typename std::enable_if<std::is_same<T, void>::value, T>::type;

		template <typename T>
		using ReturnType = typename std::enable_if<!std::is_same<T, void>::value, T>::type;


		LuaFunction(std::shared_ptr<Lua::LuaScript> script, const LuaString & name) : 
			script(script), 
			name(name), 
			functionNamePushed(false), 
			argsCount(0), 
			resCount(1)
		{ };

		~LuaFunction() = default;

		template<typename RetVal, typename T, typename ...Args>
		ReturnType<RetVal> Call(T value, Args... args);
		
		template <typename RetVal>
		ReturnType<RetVal> Call();


		template <typename RetVal>
		ReturnTypeVoid<RetVal> Call();

		template<typename RetVal, typename T, typename ...Args>
		ReturnTypeVoid<RetVal> Call(T value, Args... args);
			
	protected:
		
		std::shared_ptr<Lua::LuaScript> script;

		LuaString name;
		
		bool functionNamePushed;
		int argsCount;
		int resCount;


		bool CheckError()
		{
			if (lua_isfunction(script->state, -1) == false)
			{
				LUA_LOG_ERROR("Unknown function: %s: %s", name.c_str(),
					lua_tostring(script->GetState(), -1));

				return false;
			}

			return true;
		}

	};
	





	template<typename RetVal, typename T, typename ...Args>
	LuaFunction::ReturnType<RetVal> LuaFunction::Call(T value, Args... args)
	{
		if (this->functionNamePushed == false)
		{
			argsCount = 0;
			if (script->runCount == 0)
			{
				//lua_pcall(script->state, 0, 0, 0);
				//script->runCount++;
				script->Run();
			}

			lua_getglobal(script->state, name.c_str());  /* function to be called */

			this->functionNamePushed = true;
#ifdef _DEBUG
			if (this->CheckError() == false) return RetVal();
#endif
		}

		LuaFunctionsWrapper::Push(script->state, value);
		argsCount++;

		return this->Call<RetVal>(args...);
	}

	template <typename RetVal>
	LuaFunction::ReturnType<RetVal> LuaFunction::Call()
	{
		if (this->functionNamePushed == false)
		{
			argsCount = 0;
			if (script->runCount == 0)
			{
				//lua_pcall(script->state, 0, 0, 0);
				//script->runCount++;
				script->Run();
			}

			lua_getglobal(script->state, name.c_str());  /* function to be called */
			this->functionNamePushed = true;

#ifdef _DEBUG
			if (this->CheckError() == false) return RetVal();
#endif
		}

		if (lua_pcall(script->state, argsCount, resCount, 0) != 0)
		{
			LUA_LOG_ERROR("error running function %s: %s", name.c_str(),
				lua_tostring(script->state, -1));

			return RetVal();
		}

		RetVal v = LuaFunctionsWrapper::GetFnInput<RetVal>(script->state, -1);
		lua_pop(script->state, -1);
		this->functionNamePushed = false;

		return v;
	}

	//======================================================================

	template<typename RetVal, typename T, typename ...Args>
	LuaFunction::ReturnTypeVoid<RetVal> LuaFunction::Call(T value, Args... args)
	{
		if (this->functionNamePushed == false)
		{
			argsCount = 0;
			if (script->runCount == 0)
			{
				//lua_pcall(script->state, 0, 0, 0);
				//script->runCount++;
				script->Run();
			}

			lua_getglobal(script->state, name.c_str());  // function to be called

			this->functionNamePushed = true;
#ifdef _DEBUG
			if (this->CheckError() == false) return;
#endif
		}

		LuaFunctionsWrapper::Push(script->state, value);
		argsCount++;

		this->Call<void>(args...);
	}
	
	template <typename RetVal>
	LuaFunction::ReturnTypeVoid<RetVal> LuaFunction::Call()
	{
		if (this->functionNamePushed == false)
		{
			argsCount = 0;
			if (script->runCount == 0)
			{
				//lua_pcall(script->state, 0, 0, 0);
				//script->runCount++;
				script->Run();
			}

			lua_getglobal(script->state, name.c_str());  // function to be called
			this->functionNamePushed = true;

#ifdef _DEBUG
			if (this->CheckError() == false) return;
#endif
		}

		if (lua_pcall(script->state, argsCount, resCount, 0) != 0)
		{
			LUA_LOG_ERROR("error running function %s: %s", name.c_str(),
				lua_tostring(script->state, -1));

			return;
		}

		//RetVal v = LuaFunctionsWrapper::GetFnInput<RetVal>(script->state, -1);
		lua_pop(script->state, -1);
		this->functionNamePushed = false;
	}

	
}

#endif

