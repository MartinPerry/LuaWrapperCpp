#ifndef LUA_CLASS_BIND_H
#define LUA_CLASS_BIND_H

namespace Lua
{
	class LuaScript;
}

extern "C"
{
	#include "./lua_lib/lauxlib.h"
}

#include <string>
#include <functional>

#include "../Strings/MyStringAnsi.h"

namespace Lua
{

	template <typename T>
	struct LuaClassBind
	{
		MyStringAnsi className;
		std::function<void*(LuaScript *)> ctor;
		std::function<std::string(void *)> toString;

		std::vector<luaL_Reg> methods;
		std::vector<luaL_Reg> attrs;



		LuaClassBind() : className(typeid(T).name())
		{
			methods.push_back({ 0,0 });
			attrs.push_back({ 0, 0 });
			
		}

		~LuaClassBind()
		{
			for (size_t i = 0; i < methods.size(); i++)
			{
				delete[] methods[i].name;
				methods[i].name = NULL;
			}
			methods.clear();
		}
		
		void AddMethod(const MyStringAnsi & name, lua_CFunction f)
		{
			name.FillString(methods[methods.size() - 1].name);
			methods[methods.size() - 1].func = f;
			methods.push_back({ 0,0 });
		};

		void AddAttribute(const MyStringAnsi & name, lua_CFunction f)
		{
			name.FillString(attrs[attrs.size() - 1].name);
			attrs[attrs.size() - 1].func = f;
			attrs.push_back({ 0,0 });
		};


	};

}


#endif
