#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

namespace Lua
{
	class LuaScript;
}

struct lua_State;

#include <unordered_map>
#include <unordered_set>

#include "../Strings/MyStringAnsi.h"
#include "../Macros.h"


namespace Lua
{

	class LuaWrapper
	{
	public:
		static void Initialize();
		static void Destroy();
		static LuaWrapper * GetInstance();

		LuaScript * GetScript(lua_State * state);
		bool ExistScript(lua_State * state) const;
		
		
		LuaScript * AddScript(const MyStringAnsi & scriptName, const MyStringAnsi & scriptFileName);

		void ReloadAll();

		friend class LuaScript;

	protected:

		LuaWrapper();
		~LuaWrapper();

		void Release();

		static LuaWrapper * instance;

		
		std::unordered_map<lua_State *, LuaScript *> luaScripts;
		std::unordered_map<MyStringAnsi, void * > globalVariales;


	};

}


#endif