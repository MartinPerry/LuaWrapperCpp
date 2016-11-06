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
#include <typeindex>
#include <typeinfo>

#include "./LuaMacros.h"
#include "./LuaTypes.h"


#include "./LuaUtils.h"

namespace Lua
{
	struct LuaClass
	{
		const std::type_index typeIndex;
		const LuaString ctorName;

		bool returnLightUserData;

		
		std::function<std::string(void *)> toString;

		std::vector<luaL_Reg> ctors;
		std::vector<luaL_Reg> methods;
		std::vector<luaL_RegAttr> attrs;

		friend class LuaScript;
		friend class LuaWrapper;

	protected:

		LuaClass(const LuaString & ctorName, std::type_index typeIndex) :
			typeIndex(typeIndex),
			ctorName(ctorName)
		{
		}
		
		LuaClass(const LuaClass & c) : 
			typeIndex(c.typeIndex),
			ctorName(c.ctorName)
		{
			this->returnLightUserData = c.returnLightUserData;
			this->ctors = c.ctors;
			this->toString = c.toString;
			this->methods = c.methods;
			this->attrs = c.attrs;

			this->create_new = c.create_new;
			this->garbage_collect = c.garbage_collect;
			this->to_string = c.to_string;
			this->index = c.index;
			this->new_index = c.new_index;
		}

		lua_CFunction create_new;
		lua_CFunction garbage_collect;
		lua_CFunction to_string;
		lua_CFunction index;
		lua_CFunction new_index;
	};



	template <typename T>
	struct LuaClassBind : LuaClass
	{
				
		LuaClassBind(const LuaString & ctorName) :
			LuaClass(ctorName, std::type_index(typeid(T)))			
		{
			methods.push_back({ 0,0 });
			attrs.push_back({ 0, 0 });
			ctors.push_back({ 0,0 });
			returnLightUserData = false;

			//this->create_new = LuaCallbacks::create_new_default<T>;
			this->garbage_collect = LuaCallbacks::garbage_collect<T>;
			this->to_string = LuaCallbacks::to_string<T>;
			this->index = LuaCallbacks::index<T>;
			this->new_index = LuaCallbacks::new_index<T>;
			
		}

		~LuaClassBind()
		{
			for (size_t i = 0; i < methods.size(); i++)
			{				
				LUA_SAFE_DELETE_ARRAY(methods[i].name);
			}
			methods.clear();

			for (size_t i = 0; i < ctors.size(); i++)
			{				
				LUA_SAFE_DELETE_ARRAY(ctors[i].name);
			}
			ctors.clear();

			for (size_t i = 0; i < attrs.size(); i++)
			{
				LUA_SAFE_DELETE_ARRAY(attrs[i].name);				
			}
			attrs.clear();
		}
		
		template <typename... Args>
		void SetDefaultCtor()
		{
			this->create_new = LuaCallbacks::create_new<T, Args...>;
		}
		
		template <typename... Args>
		void AddCtor(const LuaString & name)
		{		
			if (name == this->ctorName)
			{
				//user specified ctor is named same as default ctor
				return;
			}
			
			ctors[ctors.size() - 1].name = LuaUtils::CopyString(name);
			//name.FillString(ctors[ctors.size() - 1].name);
			ctors[ctors.size() - 1].func = LuaCallbacks::create_new<T, Args...>;
			ctors.push_back({ 0,0 });
		}

		void SetDefaultCtor()
		{
			this->create_new = LuaCallbacks::create_new_default<T>;
		}

		void AddMethod(const LuaString & name, lua_CFunction f)
		{
			methods[methods.size() - 1].name = LuaUtils::CopyString(name);
			//name.FillString(methods[methods.size() - 1].name);			
			methods[methods.size() - 1].func = f;
			methods.push_back({ 0,0 });
		};

		void AddAttribute(const LuaString & name, getSetFunction f)
		{
			attrs[attrs.size() - 1].name = LuaUtils::CopyString(name);
			//name.FillString(attrs[attrs.size() - 1].name);
			attrs[attrs.size() - 1].func = f;
			attrs.push_back({ 0,0 });
		};


	};

}


#endif
