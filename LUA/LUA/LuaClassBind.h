#ifndef LUA_CLASS_BIND_H
#define LUA_CLASS_BIND_H


namespace MyUtils
{

	template <typename T>
	struct LuaClassBind
	{
		MyStringAnsi className;
		std::function<void*(MyUtils::LuaScript *)> ctor;

		std::vector<luaL_Reg> methods;



		LuaClassBind() 
		{
			methods.push_back({ 0,0 });
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
		}


	};

}


#endif