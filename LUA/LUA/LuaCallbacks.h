#ifndef LUA_CALLBACKS_H
#define LUA_CALLBACKS_H

namespace Lua
{
	class LuaFunctionsWrapper;
	class LuaScript;
}

#include <typeinfo>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <memory>

extern "C"
{
#include "./lua_lib/lua.h"
#include "./lua_lib/lualib.h"
#include "./lua_lib/lauxlib.h"
}

#include "./LuaMacros.h"
#include "./LuaTypes.h"

//=============================================================================================
// Helper templates
//=============================================================================================


struct StackTop
{
	static const int CLASS = 2; //we start with 2, because 1 is class instance itself
	static const int METHOD = 1; //we start with 1, because there are no "hidden" arguments
};

template<typename T, typename... Args>
struct ClassOverloadMethod {
	template<typename Ret>
	static auto get(Ret(T::*)(Args...)) -> Ret(T::*)(Args...) {};

	template<typename Ret>
	static auto get(Ret(T::*)(Args...)const)->Ret(T::*)(Args...)const {};
};

template<typename T>
struct ClassOverloadMethod<T> {
	template<typename Ret>
	static auto get(Ret(T::*)()) -> Ret(T::*)() {};

	template<typename Ret>
	static auto get(Ret(T::*)()const)->Ret(T::*)()const {};
};

//============================================================================

template<typename... Args>
struct OverloadMethod {
	template<typename Ret>
	static auto get(Ret(*)(Args...))->Ret(*)(Args...) {};
};

template<>
struct OverloadMethod<> {
	template<typename Ret>
	static auto get(Ret(*)())->Ret(*)() {};
};

//============================================================================

template<bool, class Ty1, class Ty2> struct MyIf
{	// type is _Ty2 for assumed false
	typedef Ty2 type;
};

template<class Ty1, class Ty2> struct MyIf<true, Ty1, Ty2>
{	// type is _Ty1 for assumed true
	typedef Ty1 type;
};

template<class Ty>
struct my_decay
{
	// determines decayed version of _Ty
	typedef typename std::decay<Ty>::type Ty1;

	typedef typename MyIf<
		std::is_arithmetic<Ty1>::value, Ty1,
		typename MyIf<
			std::is_same<LuaString, Ty1>::value, Ty1,			
			Ty
		>::type
	>::type type;
};



template<int ...>
struct seq { };

template<int N, int ...S>
struct gens : gens<N - 1, N - 1, S...> { };

template<int ...S>
struct gens<0, S...> {
	typedef seq<S...> type;
};

//=============================================================================================
// Tuple manipulation
//=============================================================================================

template <typename ...Args>
struct getTuple;

template <typename T, typename ...Args>
struct getTuple<T, Args...>;

template <>
struct getTuple<>
{
	static inline std::tuple<> get(lua_State * L, int i)
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

template <class ClassType, class Res, class... Args>
struct count_class_method_args<Res(ClassType::*)(Args...) const> 
	: count_class_method_args<Res(ClassType::*)(Args...)>
{
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
	static constexpr bool IsConstThis = false;
	//typedef count_method_args<T>::value ArgsCount;
};


template <class T, class Res, class... Args>
struct class_method_info<Res(T::*)(Args...) const> 
	: class_method_info<Res(T::*)(Args...)>
{
	static constexpr bool IsConstThis = true;
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
	
	static void call(ClassType * obj, lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::CLASS);
		(obj->*MethodName)(std::forward<Args>(std::get<Is>(tmp))...);		
	}

	static Res callWithReturn(ClassType * obj, lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::CLASS);
		return (obj->*MethodName)(std::forward<Args>(std::get<Is>(tmp))...);
	}
};


template <class ClassType,
	class Res,
	class... Args,
	Res(ClassType::*MethodName)(Args...) const,
	std::size_t... Is
>
struct func_impl_class_method<Res(ClassType::*)(Args...) const, MethodName, std::index_sequence<Is...>>	
{
	static void call(ClassType * obj, lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::CLASS);
		(obj->*MethodName)(std::forward<Args>(std::get<Is>(tmp))...);
	}

	static Res callWithReturn(ClassType * obj, lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::CLASS);
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
	static constexpr bool IsConstThis = false;
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
	
	static void call(lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::METHOD);
		(MethodName)(std::forward<Args>((std::get<Is>(tmp)))...);
	}

	static Res callWithReturn(lua_State * L)
	{
		auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, StackTop::METHOD);
		return (MethodName)(std::forward<Args>((std::get<Is>(tmp)))...);
	}
};




//=============================================================================================
// Main callback struct
//=============================================================================================





//=============================================================================================
//main part of the callbacks

//=============================================================================================
// Some Defines
//=============================================================================================

#define NO_RETURN true
#define HAVE_RETURN false
#define HAVE_ARGS >
#define NO_ARGS ==

#define LUA_CLASS_SIGNATURE_NODEFAULT(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo, \
		typename RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type*, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type*, \
		typename std::enable_if <(MethodInfo::IsClassMethod == true)>::type*

#define LUA_METHOD_SIGNATURE_NODEFAULT(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo, \
		typename RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type*, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type*, \
		typename std::enable_if <(MethodInfo::IsClassMethod == false)>::type*

#define LUA_CLASS_SIGNATURE(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo = class_method_info<MethodType>, \
		typename RetVal = typename MethodInfo::RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::IsClassMethod == true)>::type* = nullptr

#define LUA_METHOD_SIGNATURE(MethodType, VOID_RET_TYPE, ARGS) typename MethodType, \
		MethodType MethodName, \
		typename MethodInfo = method_info<MethodType>, \
		typename RetVal = typename MethodInfo::RetVal, \
		typename std::enable_if <std::is_same<void, RetVal>::value == VOID_RET_TYPE>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::ArgsCount ARGS 0), void>::type* = nullptr, \
		typename std::enable_if <(MethodInfo::IsClassMethod == false)>::type* = nullptr

#define LUA_CLASS_FUNCTION_BODY \
		typename MethodInfo::ClassType *a = LuaCallbacks::GetPtr<typename MethodInfo::ClassType>(L, 1); \
		std::shared_ptr<Lua::LuaScript> script = Lua::LuaWrapper::GetInstance()->GetScript(L); \
		script->Reset();




struct LuaCallbacks
{	
	//http://stackoverflow.com/questions/29194858/order-of-function-calls-in-variadic-template-expansion
		

	//=============================================================================================
	// Class callbacks
	//=============================================================================================


	template <LUA_CLASS_SIGNATURE(MethodType, NO_RETURN, HAVE_ARGS)>
	static int function(lua_State *L);


	template <LUA_CLASS_SIGNATURE(MethodType, HAVE_RETURN, HAVE_ARGS)>
	static int function(lua_State *L);


	template <LUA_CLASS_SIGNATURE(MethodType, NO_RETURN, NO_ARGS)>
	static int function(lua_State *L);


	template <LUA_CLASS_SIGNATURE(MethodType, HAVE_RETURN, NO_ARGS)>
	static int function(lua_State *L);

	
	//=============================================================================================
	// Method callbacks
	//=============================================================================================

	

	template <LUA_METHOD_SIGNATURE(MethodType, NO_RETURN, HAVE_ARGS)>
	static int function(lua_State *L);

	template <LUA_METHOD_SIGNATURE(MethodType, HAVE_RETURN, HAVE_ARGS)>
	static int function(lua_State *L);

	template <LUA_METHOD_SIGNATURE(MethodType, NO_RETURN, NO_ARGS)>
	static int function(lua_State *L);

	template <LUA_METHOD_SIGNATURE(MethodType, HAVE_RETURN, NO_ARGS)>
	static int function(lua_State *L);

	//=============================================================================================
	// Attribute callbacks
	//=============================================================================================
	template <typename T, class ClassType, T ClassType::*MemPtr>
	static int getSetAttr(lua_State *L, AttrCallType type);

	
	
	//=============================================================================================
	// Other methods
	//=============================================================================================


//protected:

	static std::unordered_map<std::type_index, LuaString> tableName;	
	static std::unordered_map<std::type_index, std::shared_ptr<LuaClassToString>> toString;

	template <typename T>
	static T * GetPtr(lua_State *L, int narg);

	template<typename T, typename... Args, int ...S>
	static T * ctor(lua_State * L, seq<S...>);


	template <typename T, typename... Args>
	static int create_new(lua_State * L);

	

	template <typename T>
	LUA_INLINE static void SetNewUserDataClass(lua_State *L, T * val);




	template <typename T>
	static int garbage_collect(lua_State *L);

	template <typename T>
	static int to_string(lua_State *L);

	
	
	template <typename T>
	static int new_index(lua_State *L);

	template <typename T>
	static int index(lua_State *L);

};


#include "./LuaFunctionWrapper.h"
#include "./LuaWrapper.h"
#include "./LuaScript.h"


//http://stackoverflow.com/questions/29194858/order-of-function-calls-in-variadic-template-expansion


//=============================================================================================
// Class callbacks
//=============================================================================================


template <LUA_CLASS_SIGNATURE_NODEFAULT(MethodType, NO_RETURN, HAVE_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	typename MethodInfo::ClassType *a = LuaCallbacks::GetPtr<typename MethodInfo::ClassType>(L, 1);

	func_impl_class_method<MethodType, MethodName>::call(a, L);

	return 0;
}


template <LUA_CLASS_SIGNATURE_NODEFAULT(MethodType, HAVE_RETURN, HAVE_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	LUA_CLASS_FUNCTION_BODY;

	script->Push(func_impl_class_method<MethodType, MethodName>::callWithReturn(a, L));

	return script->GetFnReturnValueCount();
}


template <LUA_CLASS_SIGNATURE_NODEFAULT(MethodType, NO_RETURN, NO_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	typename MethodInfo::ClassType *a = LuaCallbacks::GetPtr<typename MethodInfo::ClassType>(L, 1);

	(a->*MethodName)();

	return 0;
}


template <LUA_CLASS_SIGNATURE_NODEFAULT(MethodType, HAVE_RETURN, NO_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	LUA_CLASS_FUNCTION_BODY;

	script->Push((a->*MethodName)());

	return script->GetFnReturnValueCount();
}


//=============================================================================================
// Method callbacks
//=============================================================================================



template <LUA_METHOD_SIGNATURE_NODEFAULT(MethodType, NO_RETURN, HAVE_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	func_impl_method<MethodType, MethodName>::call(L);

	return 0;
}

template <LUA_METHOD_SIGNATURE_NODEFAULT(MethodType, HAVE_RETURN, HAVE_ARGS)>
int LuaCallbacks::function(lua_State *L)
{

	std::shared_ptr<Lua::LuaScript> script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	script->Reset();

	script->Push(func_impl_method<MethodType, MethodName>::callWithReturn(L));

	return script->GetFnReturnValueCount();
}

template <LUA_METHOD_SIGNATURE_NODEFAULT(MethodType, NO_RETURN, NO_ARGS)>
int LuaCallbacks::function(lua_State *L)
{
	(MethodName)();

	return 0;
}

template <LUA_METHOD_SIGNATURE_NODEFAULT(MethodType, HAVE_RETURN, NO_ARGS)>
int LuaCallbacks::function(lua_State *L)
{

	std::shared_ptr<Lua::LuaScript> script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	script->Reset();

	script->Push((MethodName)());

	return script->GetFnReturnValueCount();
}

//=============================================================================================
// Attribute callbacks
//=============================================================================================
template <typename T, class ClassType, T ClassType::*MemPtr>
int LuaCallbacks::getSetAttr(lua_State *L, AttrCallType type)
{
	ClassType *a = LuaCallbacks::GetPtr<ClassType>(L, 1);
	std::shared_ptr<Lua::LuaScript> script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	script->Reset();

	if (type == AttrCallType::GET)
	{
		script->Push(a->*MemPtr);
	}
	else if (type == AttrCallType::SET)
	{
		a->*MemPtr = Lua::LuaFunctionsWrapper::GetFnInput<T>(L, StackTop::CLASS);
	}


	return script->GetFnReturnValueCount();
}



//=============================================================================================
// Other methods
//=============================================================================================


template <typename T>
T * LuaCallbacks::GetPtr(lua_State *L, int narg)
{
	//Lua::LuaScript * script = Lua::LuaWrapper::GetInstance()->GetScript(L);
	//script->PrintStack("GetPTR");

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
			LUA_LOG_ERROR("Type ad the stack top is not LUA_TUSERDATA");
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
		LUA_LOG_ERROR("Type at the stack top is not LUA_TUSERDATA OR LUA_TLIGHTUSERDATA");
		return NULL;
	}
}

template<typename T, typename... Args, int ...S>
T * LuaCallbacks::ctor(lua_State * L, seq<S...>)
{
	auto tmp = getTuple<typename my_decay<Args>::type...>::get(L, 1); //we start with 1 -> ctor in Lua is call of regular method
	return new T(std::forward<Args>((std::get<S>(tmp)))...);
}


template <typename T, typename... Args>
int LuaCallbacks::create_new(lua_State * L)
{
	T * newData = LuaCallbacks::ctor<T, Args...>(L, typename gens<sizeof...(Args)>::type());

	LuaCallbacks::SetNewUserDataClass(L, newData);

	return 1;
}



template <typename T>
LUA_INLINE void LuaCallbacks::SetNewUserDataClass(lua_State *L, T * val)
{

	T ** udata = (T **)lua_newuserdata(L, sizeof(T *));
	*udata = val;

	const char * classTableName = LuaCallbacks::tableName[std::type_index(typeid(T))].c_str();

	luaL_getmetatable(L, classTableName);

	lua_setmetatable(L, -2);
}




template <typename T>
int LuaCallbacks::garbage_collect(lua_State *L)
{
	LUA_LOG_INFO("Garbage Collect");

	int argType = lua_type(L, 1);
	if (argType == LUA_TUSERDATA)
	{
		T* a = (*(T **)(lua_touserdata(L, 1)));
		delete a;
		a = nullptr;
	}
	return 0;
}

template <typename T>
int LuaCallbacks::to_string(lua_State *L)
{
	T * a = LuaCallbacks::GetPtr<T>(L, 1);
	auto it = LuaCallbacks::toString.find(std::type_index(typeid(T)));
	if (it == LuaCallbacks::toString.end())
	{
		lua_pushfstring(L, "[__tostring userdata = address: %p]", a);
	}
	else if (it->second == nullptr)
	{
		lua_pushfstring(L, "[__tostring userdata = address: %p]", a);
	}
	else
	{
		LuaString str = (it->second)->Call(a);
		lua_pushfstring(L, "%s", str.c_str());
	}
	return 1;
}



template <typename T>
int LuaCallbacks::new_index(lua_State *L)
{
	const char * metatableName = lua_tostring(L, lua_upvalueindex(1));

	lua_insert(L, 1);

	const char * keyName = luaL_checkstring(L, -1);

	luaL_getmetatable(L, metatableName);
	lua_getfield(L, -1, keyName);
	getSetFunction getSetArg = (getSetFunction)lua_touserdata(L, -1);

	if (getSetArg == NULL)
	{
		return 1;
	}


	lua_settop(L, 2);  //remove everything	except last 2 values from stack
	lua_insert(L, 1);  //"put" stack top value on stack bottom

	getSetArg(L, AttrCallType::SET);

	return 0;
}

template <typename T>
int LuaCallbacks::index(lua_State *L)
{
	const char * metatableName = lua_tostring(L, lua_upvalueindex(1));
	const char * keyName = luaL_checkstring(L, -1);

	luaL_getmetatable(L, metatableName); //set metatable on stack top
	lua_getfield(L, -1, keyName); //get "keyName" value on top
	getSetFunction getSetArg = (getSetFunction)lua_touserdata(L, -1);

	if (getSetArg != NULL)
	{
		getSetArg(L, AttrCallType::GET);
	}


	return 1;
}

template <typename T, typename ...Args>
struct getTuple<T, Args...>
{
	static inline std::tuple<T, Args...> get(lua_State * L, int i)
	{

		/*
		std::tuple<T> t = std::make_tuple<T>(script->GetFnInput<T>());
		std::tuple<Args...> args = getTuple<Args...>::get(script);
		return std::tuple_cat(t, args);
		*/

		std::tuple<T> t = std::forward_as_tuple(Lua::LuaFunctionsWrapper::GetFnInput<T>(L, i));
		std::tuple<Args...> args = getTuple<Args...>::get(L, i + 1);
		return std::tuple_cat(std::move(t), std::move(args));

		/*
		//reversed order
		std::tuple<Args...> args = getTuple<Args...>::get(script);
		std::tuple<T> t = std::make_tuple<T>( script->GetFnInput<T>());
		return std::tuple_cat(t, args);
		*/
	}
};

#endif
