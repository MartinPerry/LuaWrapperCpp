#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

namespace Lua
{
	class LuaScript;
}

struct lua_State;

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "./LuaMacros.h"
#include "./LuaTypes.h"
#include "./LuaScript.h"
#include "./LuaClassBind.h"

namespace Lua
{

	class LuaWrapper
	{
	public:
		static void Initialize(ScriptLoaderCallback loaderCallback);
		static void Destroy();
		static LuaWrapper * GetInstance();


		void SetRegisterCallback(RegisterCallback regCallback);

		void AddClass(const LuaClass & luaClass);

		std::shared_ptr<LuaScript> GetScript(lua_State * state);
		bool ExistScript(lua_State * state) const;
		
		
		std::shared_ptr<LuaScript> AddScript(const LuaString & scriptFileName);
		std::shared_ptr<LuaScript> AddScript(const LuaString & scriptName, const LuaString & scriptFileName);

		
		friend class LuaScript;

	protected:

		RegisterCallback regCallback;
		ScriptLoaderCallback scriptLoaderCallback;

		LuaWrapper();
		~LuaWrapper();

		void Release();

		static LuaWrapper * instance;

		LuaString GetScriptFromFile(const LuaString & scriptFileName);

		std::unordered_map<lua_State *, std::shared_ptr<LuaScript>> luaScripts;
		//std::unordered_map<MyStringAnsi, void * > globalVariales;

		std::unordered_map<LuaString, const LuaClass *> classes;


	};

}


#endif