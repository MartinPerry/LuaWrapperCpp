#ifndef LUA_CALLBACKS_H
#define LUA_CALLBACKS_H


#include <typeinfo>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <string.h>


#include "../Strings/MyConstString.h"

#include "./LuaScript.h"
#include "./LuaMacros.h"

//=============================================================================================
// Helper templates
//=============================================================================================


template<class _Ty>
struct my_decay
{
	// determines decayed version of _Ty
	typedef typename std::decay<_Ty>::type _Ty1;

	typedef typename std::_If<
		std::is_arithmetic<_Ty1>::value, typename _Ty1,
		typename std::_If<
			std::is_same<MyStringAnsi, _Ty1>::value, typename _Ty1,
			typename _Ty
		>::type
	>::type type;
};


//=============================================================================================
// Tuple manipulation
//=============================================================================================

template <typename ...Args>
struct getTuple;

template <typename T, typename ...Args>
struct getTuple<T, Args...>
{
	static inline std::tuple<T, Args...> get(Lua::LuaScript * script)
	{
		/*
		std::tuple<T> t = std::make_tuple<T>(script->GetFnInput<T>());
		std::tuple<Args...> args = getTuple<Args...>::get(script);
		return std::tuple_cat(t, args);
		*/

		std::tuple<T> t = std::forward_as_tuple(script->GetFnInput<T>());
		std::tuple<Args...> args = getTuple<Args...>::get(script);
		return std::tuple_cat(std::move(t), std::move(args));

		/*
		//reversed order
		std::tuple<Args...> args = getTuple<Args...>::get(script);
		std::tuple<T> t = std::make_tuple<T>( script->GetFnInput<T>());
		return std::tuple_cat(t, args);
		*/
	}
};

template <>
struct getTuple<>
{
	static inline std::tuple<> get(Lua::LuaScript * script)
	{
		return std::make_tuple<>();
	}
};


//=================================================================================================
// Helper templates for class
//=================================================================================================


template <class MT>
struct count_class_method_args;

template <class ClassType, class Res, class... Args>
struct count_class_method_args<Res(ClassType::*)(Args...)>
{
	static constexpr std::size_t value = sizeof...(Args);
};


//=======================================================================================

template <class MT>
struct class_method_info;

template <class T, class Res, class... Args>
struct class_method_info<Res(T::*)(Args...)>
{
	typedef std::tuple<Args&&...> ArgsTuple;
	typedef T ClassType;
	typedef Res RetVal;
	static constexpr std::size_t ArgsCount = sizeof...(Args);
	static constexpr bool IsClassMethod = true;	
	//typedef count_method_args<T>::value ArgsCount;
};

//=======================================================================================





template <class MT, MT, class I = std::make_index_sequence<count_class_method_args<MT>::value>>
struct func_impl_class_method;

template <class ClassType,
	class Res,
	class... Args,
	Res(ClassType::*MethodName)(Args...),
	std::size_t... Is
>
struct func_impl_class_method<Res(ClassType::*)(Args...), MethodName, std::index_sequence<Is...>>
{
	/*
	void operator()(ClassType * obj, std::tuple<Args...> &&tup)
	{
	(obj->*MethodName)(std::forward<Args>((std::get<Is>(tup)))...);
	}
	*/

	static void call(ClassType * obj, Lua::LuaScript * s)
	{
		//auto tmp = getTuple<typename std::decay<Args>::type...>::get(s);
		//auto tmp = getTuple<Args&&...>::get(s);

		auto tmp = getTuple<typename my_decay<Args>::type...>::get(s);
		//auto tmp = getTuple2<typename std::decay<Args&&>::type...>::get(s);
		//auto tmp = getTuple2<typename std::decay<Args>::type&&...>::get(s);
		//auto tmp = getTuple2<Args...>::get(s);
		
		//tuple_extractor<sizeof...(Args)>::extractTuple<ClassType, Res(ClassType::*)(Args...), Args...>(obj, MethodName, tmp);
		
		(obj->*MethodName)(std::forward<Args>(std::get<Is>(tmp))...);		
	}

	static Res callWithReturn(ClassType * obj, Lua::LuaScript * s)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(s);

		//tuple_extractor<sizeof...(Args)>::extractTuple<ClassType, Res(ClassType::*)(Args...), Args...>(obj, MethodName, tmp);

		return (obj->*MethodName)(std::forward<Args>(std::get<Is>(tmp))...);
	}
};


//=================================================================================================
// Helper templates for method
//=================================================================================================


template <class MT>
struct count_method_args;

template <class Res, class... Args>
struct count_method_args<Res(*)(Args...)>
{
	static constexpr std::size_t value = sizeof...(Args);
};

//=======================================================================================

template <class MT>
struct method_info;

template <class Res, class... Args>
struct method_info<Res(*)(Args...)>
{
	typedef std::tuple<Args&&...> ArgsTuple;
	typedef Res RetVal;
	static constexpr std::size_t ArgsCount = sizeof...(Args);
	static constexpr bool IsClassMethod = false;	
	//typedef count_method_args<T>::value ArgsCount;
};

//=======================================================================================

template <class MT, MT, class I = std::make_index_sequence<count_method_args<MT>::value>>
struct func_impl_method;

template <class Res,
	class... Args,
	Res(*MethodName)(Args...),
	std::size_t... Is
>
struct func_impl_method<Res(*)(Args...), MethodName, std::index_sequence<Is...>>
{
	
	static void call(Lua::LuaScript * s)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(s);
		(MethodName)(std::forward<Args>((std::get<Is>(tmp)))...);
	}

	static Res callWithReturn(Lua::LuaScript * s)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(s);
		return (MethodName)(std::forward<Args>((std::get<Is>(tmp)))...);
	}
};





//=============================================================================================
// Main callback struct
//=============================================================================================





//=============================================================================================
//main part of the callbacks


struct LuaCallbacks
{	
	//http://stackoverflow.com/questions/29194858/order-of-function-calls-in-variadic-template-expansion

	//=============================================================================================
	// Some Defines
	//=============================================================================================

#define NO_RETURN true
#define HAVE_RETURN false
#define HAVE_ARGS >
#define NO_ARGS ==

#define CLASS_SIGNATURE(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo = class_method_info<MethodType>, \
		typename RetVal = MethodInfo::RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::IsClassMethod == true)>::type* = nullptr

#define METHOD_SIGNATURE(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo = method_info<MethodType>, \
		typename RetVal = MethodInfo::RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::IsClassMethod == false)>::type* = nullptr

#define CLASS_FUNCTION_BODY \
		MethodInfo::ClassType *a = LuaCallbacks::GetPtr<MethodInfo::ClassType>(L, 1); \
		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L); \
		script->Reset(); \
		script->IncStack(); 

	//=============================================================================================
	// Class callbacks
	//=============================================================================================


	template <CLASS_SIGNATURE(MethodType, NO_RETURN, HAVE_ARGS)>
	static int function(lua_State *L)
	{		
		CLASS_FUNCTION_BODY;

		func_impl_class_method<MethodType, MethodName>::call(a, script);

		return script->GetFnReturnValueCount();
	}


	template <CLASS_SIGNATURE(MethodType, HAVE_RETURN, HAVE_ARGS)>	
	static int function(lua_State *L)
	{
		CLASS_FUNCTION_BODY;

		script->AddFnReturnValue(func_impl_class_method<MethodType, MethodName>::callWithReturn(a, script));

		return script->GetFnReturnValueCount();
	}


	template <CLASS_SIGNATURE(MethodType, NO_RETURN, NO_ARGS)>
	static int function(lua_State *L)
	{
		CLASS_FUNCTION_BODY;
		
		(a->*MethodName)();

		return script->GetFnReturnValueCount();
	}

	template <CLASS_SIGNATURE(MethodType, HAVE_RETURN, NO_ARGS)>
	static int function(lua_State *L)
	{
		CLASS_FUNCTION_BODY;

		script->AddFnReturnValue((a->*MethodName)());

		return script->GetFnReturnValueCount();
	}

	//=============================================================================================
	// Method callbacks
	//=============================================================================================

	

	template <METHOD_SIGNATURE(MethodType, NO_RETURN, HAVE_ARGS)>
	static int function(lua_State *L)
	{
		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset(); 
				
		func_impl_method<MethodType, MethodName>::call(script);

		return script->GetFnReturnValueCount();
	}

	template <METHOD_SIGNATURE(MethodType, HAVE_RETURN, HAVE_ARGS)>
	static int function(lua_State *L)
	{

		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset();
		
		script->AddFnReturnValue(func_impl_method<MethodType, MethodName>::callWithReturn(script));

		return script->GetFnReturnValueCount();
	}

	template <METHOD_SIGNATURE(MethodType, NO_RETURN, NO_ARGS)>	
	static int function(lua_State *L)
	{

		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset();
				
		(MethodName)();

		return script->GetFnReturnValueCount();
	}

	template <METHOD_SIGNATURE(MethodType, HAVE_RETURN, NO_ARGS)>
	static int function(lua_State *L)
	{

		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset();
				
		script->AddFnReturnValue((MethodName)());

		return script->GetFnReturnValueCount();
	}

	//=============================================================================================
	// Attribute callbacks
	//=============================================================================================
	template <typename T, class ClassType, T ClassType::*MemPtr>
	static int getAttr(lua_State *L)
	{
		ClassType *a = LuaCallbacks::GetPtr<ClassType>(L, 1);
		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset();
		script->IncStack();
				
		script->AddFnReturnValue(a->*MemPtr);

		return script->GetFnReturnValueCount();		
	}
	

	
	//=============================================================================================
	// Other methods
	//=============================================================================================


//protected:

	static std::unordered_map<MyConstString, std::function<void*(Lua::LuaScript *)> > ctors;
	static std::unordered_map<MyConstString, std::function<std::string(void *)>> toString;

	template <typename T>
	static T * GetPtr(lua_State *L, int narg)
	{
		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->PrintStack("GetPTR");

		int argType = lua_type(L, narg);

		if (argType == LUA_TUSERDATA)
		{
			void * ud = NULL;			
#if (SAFE_PTR_CHECKS == 1)	
			ud = luaL_checkudata(L, narg, lua_tostring(L, lua_upvalueindex(1)));
#else
			ud = lua_touserdata(L, narg); //"unsafe" - can run in "release", 
										  // if we check bugs in debug
#endif
			if (!ud)
			{
				//luaL_typerror(L, narg, className);
				MY_LOG_ERROR("Type ad the stack top is not LUA_TUSERDATA");
				return NULL;
			}			
			return   *(T **)ud;  // unbox pointer
		}
		else if (argType == LUA_TLIGHTUSERDATA)
		{
			void * data = lua_touserdata(L, narg);
			return static_cast<T *>(data);
		}
		else
		{
			MY_LOG_ERROR("Type at the stack top is not LUA_TUSERDATA OR LUA_TLIGHTUSERDATA");
			return NULL;
		}
	}

	template <typename T>
	static int create_new(lua_State *L)
	{
		std::function<void*(Lua::LuaScript *)> f = LuaCallbacks::ctors[MyConstString(typeid(T).name())];

		Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
		script->Reset();

		/*
		lua_newtable(L); //mt1
		
		// push a temporary variable into the imt
		lua_pushstring(L, "instancevar");
		lua_pushnumber(L, 0.5);
		lua_settable(L, -3);

		// set mt1.__index = mt1
		lua_pushstring(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		// set mt1.__newindex = mt1
		lua_pushstring(L, "__newindex");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

				
		// get class metatable (mt2) & assign to instance metatable (mt1)
		luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));
		lua_setmetatable(L, -2);		// setmetatable(mt1, mt2)

		//luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));
		//lua_insert(L, 1);
		//lua_setmetatable(L, -2);		// setmetatable(mt2, mt1)

		MyStringAnsi atrName = lua_tostring(L, lua_upvalueindex(1));
		atrName += "_attrs";
		
		

		T ** udata = (T **)lua_newuserdata(L, sizeof(T *));				
		*udata = static_cast<T *>(f(script));
		
		//script->PrintStack("Bef");
		lua_insert(L, 1);				
		//script->PrintStack("Aft");
		lua_setmetatable(L, -2);		// setmetatable(udata, mt1)
		script->PrintStack("Aft3");

		//luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));
		//lua_setmetatable(L, -2);		// setmetatable(udata, mt2)

		*/

		T * newData = static_cast<T *>(f(script));;
						
		LuaCallbacks::SetNewUserDataClass(L, newData, lua_tostring(L, lua_upvalueindex(1)));
		
		/*

		


		T ** udata = (T **)lua_newuserdata(L, sizeof(T *));
		*udata = newData;
		printf("New daza: %p %p\n", udata, *udata);

		luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));		
		script->PrintStack("MT2");

		//put the "pointer to data" into "arguments" table
		lua_pushstring(L, "__parent");
		lua_pushlightuserdata(L, *udata);
		lua_settable(L, -3);


		lua_setmetatable(L, -2);
		
		script->PrintStack("New user data");
		*/
		

		return 1;

	}

	template <typename T>
	LUA_INLINE static void SetNewUserDataClass(lua_State *L, T * val, const MyStringAnsi & className)
	{
		T ** udata = (T **)lua_newuserdata(L, sizeof(T *));
		*udata = val;

		luaL_getmetatable(L, className.c_str());

		//put the "pointer to data" into "arguments" table
		lua_pushstring(L, "__parent");
		lua_pushlightuserdata(L, *udata);
		lua_settable(L, -3);


		lua_setmetatable(L, -2);
	}




	template <typename T>
	static int garbage_collect(lua_State *L)
	{
		MY_LOG_INFO("Garbage Collect");

		int argType = lua_type(L, 1);
		if (argType == LUA_TUSERDATA)
		{			
			T* a = (*(T **)(lua_touserdata(L, 1)));
			delete a;
			a = NULL;
		}
		return 0;
	}

	template <typename T>
	static int to_string(lua_State *L)
	{
		T *a = LuaCallbacks::GetPtr<T>(L, 1);
		std::function<std::string(void *)> f = LuaCallbacks::toString[MyConstString(typeid(T).name())];
		if (f == nullptr)
		{
			lua_pushfstring(L, "[__tostring userdata = address: %p]", a);
		}
		else
		{
			lua_pushfstring(L, "%s", f(a).c_str());
		}
		return 1;
	}


	template <typename T>
	static int new_index(lua_State *L)
	{
		T *a = LuaCallbacks::GetPtr<T>(L, 1);

		int metatable = lua_getmetatable(L, 2);
		if (lua_isnumber(L, 3)) 
		{
			int v = lua_tointeger(L, 3);
			printf("cislo ");
		}
		

		printf("new index...\n");
		return 0;
	}

	static int tmp(lua_State * L);

	template <typename T>
	static int index(lua_State *L)
	{
		return tmp(L);
		printf("GET INDEX\n");
		return 0;
		const char * keyName = luaL_checkstring(L, -1);		
		
		luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));
		
		lua_getfield(L, -1, keyName);

		lua_CFunction getArg = (lua_CFunction)lua_touserdata(L, -1);
		
		

		if (getArg == NULL)
		{
			printf("????");
			return 1;
		}
		lua_pop(L, 1);
		lua_pop(L, 1);
		
					
		getArg(L);
	
		return 1;
	}

};




#endif
