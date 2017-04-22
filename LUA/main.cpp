
#define MAIN_MAIN
#ifdef MAIN_MAIN

#include "./LUA/LuaWrapperCpp.h"


#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <array>
#include <tuple>
#include <vector>

#include "Benchmarks.h"

//=================================================================================================
//=================================================================================================
//=================================================================================================

//https://github.com/SteveKChiu/lua-intf



//=================================================================================================
//=================================================================================================
//=================================================================================================
#include "./TestClass.h"

//=================================================================================================
//=================================================================================================
//=================================================================================================
/*
void HelloMethod()
{
	printf("Hello method\n");
}

void HelloMethodParam(int v)
{
	printf("Hello method Param: %i\n", v);
}

int HelloMethodParamReturn(int v)
{
	//printf("Hello method Param: %i\n", v);
	return v + 1;
}


struct BaseType
{
	virtual BaseType* Clone() const = 0;
};

template <typename DT>
struct DerivedType : public BaseType
{
	template <typename U>
	DerivedType(U &&  value)
		: value(std::forward<U>(value))
	{
	}

	BaseType* Clone() const { return new DerivedType<DT>(value); }

	DT value; //decayed type
};


struct AnyType
{
public:
	
	AnyType()
		: ptr(nullptr)
	{

	}	

	AnyType(AnyType& that)
		: ptr(that.Clone())
	{

	}

	
	~AnyType()
	{
		delete ptr;
		ptr = NULL;
	};
	

	AnyType& operator=(const AnyType& a)
	{
		if (ptr == a.ptr)
		{
			return *this;
		}

		BaseType * old_ptr = ptr;

		ptr = a.Clone();

		if (old_ptr)
		{
			delete old_ptr;
		}
		return *this;
	}

	template <typename T>
	AnyType& operator=(T & a)
	{		
		BaseType * old_ptr = ptr;

		if (old_ptr)
		{
			delete old_ptr;
		}

		this->ptr = new DerivedType<std::decay<T>::type>(std::forward<T>(a));
		return *this;
	}

	
	

	template <typename T>
	T & GetAs()
	{				
		auto derived = dynamic_cast<DerivedType<std::decay<T>::type> *>(ptr);

		if (!derived) 
		{
			throw std::bad_cast();
		}

		return derived->value;
	};
	


private:

	BaseType* Clone() const
	{
		return (ptr) ? ptr->Clone() : nullptr;		
	}

	BaseType * ptr;

};



*/

//==================================================================================================
//==================================================================================================
//==================================================================================================




template<typename T, typename ... Args>
using get_print_type = decltype(std::declval<T>().Print0(std::declval<Args>() ...)) (T::*)(Args ...);


std::shared_ptr<Lua::LuaScript> Create(LuaString name)
{
	//http://loadcode.blogspot.cz/2007/02/wrapping-c-classes-in-lua.html
	//https://john.nachtimwald.com/2014/07/12/wrapping-a-c-library-in-lua/

	std::shared_ptr<Lua::LuaScript> ls = Lua::LuaWrapper::GetInstance()->AddScriptFromFile(name, name);

	//using bar_t = TypeOverload<Account, &Account::Print5, int>::type;

	/*
	using bar_t = TypeOverload<Foo, void(Foo::*)(int, int), &Foo::bar, int, int>::type;
	static_assert(std::is_same<bar_t, void(Foo::*)(int, int)>::value, "y");

	using baz_t = TypeOverload<Foo, int(Foo::*)(double), &Foo::baz, double>::type;
	static_assert(std::is_same<baz_t, int(Foo::*)(double)>::value, "x");
	*/

	

	//ls->RegisterFunction("Print_fce", NULL);
	


	Lua::LuaClassBind<Vector2> cbv2("Vector2");
	cbv2.SetDefaultCtor<>();
	cbv2.AddAttribute("x", CLASS_ATTRIBUTE(Vector2, x));
	cbv2.AddAttribute("y", CLASS_ATTRIBUTE(Vector2, y));
	cbv2.SetToString([](Vector2 * a) -> LuaString {

		std::string str = "Vector2[";
		str += std::to_string(a->x);
		str += ", ";
		str += std::to_string(a->y);
		str += "]";

		return str;
	});

	ls->RegisterLuaClass(cbv2);

	Lua::LuaClassBind<Account> cb("Account");
	cb.SetDefaultCtor<double>();
	cb.AddCtor<double, double>("Account_2");
	//cb.AddMethod("Print0", LuaCallbacks::function<decltype(std::declval<Account>().Print0()) (Account::*)(), &Account::Print0>);
	cb.AddMethod("Print0", CLASS_METHOD_OVERLOAD(BaseAccount, Print0));
	cb.AddMethod("Print0L", CLASS_METHOD_OVERLOAD(BaseAccount, Print0, double, double));
	//cb.AddMethod("Print0L", LuaCallbacks::function<TMP(Account, Print0, double, double), &Account::Print0>);
	
	//cb.AddMethod("Print0L", LuaCallbacks::function<TypeOverload<Account, &Account::Print0, double, double>::type, &Account::Print0>);
	cb.AddMethod("Print1", CLASS_METHOD(Account, Print1));
	cb.AddMethod("Print2", CLASS_METHOD(Account, Print2));
	cb.AddMethod("Print3", CLASS_METHOD(Account, Print3));
	cb.AddMethod("Print4", CLASS_METHOD(Account, Print4));
	cb.AddMethod("PrintConst", CLASS_METHOD(Account, PrintConst));
	//cb.AddMethod("PrintConst", LuaCallbacks::function2<decltype(&Account::PrintConst), &Account::PrintConst>);	
	cb.AddMethod("Print5", CLASS_METHOD(Account, Print5));
	cb.AddMethod("Print6", CLASS_METHOD(Account, Print6));
	cb.AddMethod("Print7", CLASS_METHOD(Account, Print7));
	cb.AddMethod("deposit", CLASS_METHOD(Account, deposit));
	cb.AddMethod("balance", CLASS_METHOD(Account, balance));
	

	cb.AddAttribute("vv", CLASS_ATTRIBUTE(BaseAccount, val));
	cb.AddAttribute("xx", CLASS_ATTRIBUTE(Account, xx));
	cb.AddAttribute("cc", CLASS_ATTRIBUTE(Account, cc));
	cb.AddAttribute("v2", CLASS_ATTRIBUTE(Account, v2));


	cb.SetToString([](Account * a) -> LuaString {
		
		std::string str = "to_string_";
		str += std::to_string(a->val);

		return str;
	});


	ls->RegisterLuaClass(cb);

		


	//ls->RegisterLuaFunction("Print_fce", METHOD(HelloMethod));
	//ls->RegisterLuaFunction("Print_fce2", METHOD(HelloMethodParam));


	
	Lua::LuaClassBind<Account2> cb2("Account2");	
	cb.SetDefaultCtor<double>();
	cb2.AddMethod("Print0", CLASS_METHOD(Account2, Print0));
	

	//ls->RegisterClass<Account2>(cb2);
	

	return ls;
}

/*
struct MyClass
{
	int i;
	char c;
};


template<typename T, class C, T C::*MemPtr>
void doSomething(MyClass * cl)
{
	(cl->*MemPtr)++;
}

template<typename T, T t>
void doSomething2()
{
	//(cl->*MemPtr)++;
}


float ConvertXY(float x)
{
	return std::sin(x);
}
*/


int main(int argc, char * argv[])
{
	
	
	

	/*
	MyClass cl;
	cl.i = 7;
	//arg_info<decltype(MyClass::i)> o();
	doSomething2<&MyClass::i>();
	doSomething<decltype(MyClass::i), MyClass, &MyClass::i>(&cl);


	return 0;
	*/
	
	Lua::LuaWrapper::Initialize([](const LuaString & str) -> LuaString {
		return Lua::LuaUtils::LoadFromFile(str);
	});

	
	//
	RunBenchmarkLuaFromCpp();

	RunBenchmarkCppFromLua();


	/*
	Account * a = new Account(150);
	int aa = 100;

	//AnyType t1 = AnyType(a);
	AnyType t1;
	t1 = a;
	//t1.CreateAnyType(a);
	

	//AnyType t2 = AnyType(aa);

	Account * a1 = t1.GetAs<Account *>();
	t1.~AnyType();
	//int aa1 = t2.GetAs<int>();

	return 1;
	*/


	
	//LuaString script = "print(\"\" .. math.sin(x))";
	LuaString script = "function test1(x, y) return x + y end function ss(x) return math.sin(x) end";

	
	std::shared_ptr<Lua::LuaScript> ls2 = Lua::LuaWrapper::GetInstance()->AddScript("xxx", script);
	//ls2->Run();

	

	Lua::LuaFunction lf2(ls2, "ss");

	//lf.Call<int>();
	float r2 = lf2.Call<float>(0.5);
	float r3 = lf2.Call<float>(0.1);

	printf("xx");

	//return 0;


	ls2->SetGlobalVar("x", 11);
	ls2->Run();

	
	//return 0;
	
	std::shared_ptr<Lua::LuaScript> ls = Create("t2.lua");
	

	
	//lua_boxpointer(L, aa);
	//(*(void **)(lua_newuserdata(L, sizeof(void *))) = (aa));
	
	//create data in "lua" memory
	//Account *foo = new (lua_newuserdata(L, sizeof(Account))) Account(450);

	//void * luaMem = lua_newuserdata(L, sizeof(void *));
	//luaMem = aa;

	
	Account* ee = new Account(600);	
	ls->SetGlobalVarClass("ee", ee);

	Account* cc = new Account(900);
	//ls->SetGlobalVarLight("cc", cc);

	ls->Run();
	printf("BBB %f BBB\n", cc->balance());
	printf("BBB %f BBB\n", ee->balance());
	printf("---> %f\n", ee->val);
	return 0;
	/*
	//ls->RegisterFunction("LUA_Test", LUA_Test);		
	ls->RegisterFunction("LUA_Test", LUA_TestClass);
	START;
	ls->Run();
	END;
	PRINT_TIME;
	


	MyUtils::LuaScript *ls2 = MyUtils::LuaWrapper::GetInstance()->AddScriptFromFile("t.lua", "t.lua");
	
	MyFunction<int, TestClass *, int> * ff = new MyFunction<int, TestClass *, int>();
	ff->f = &HelloMethodParamReturn2;

	f = ff;
	//ls2->RegisterFunction("LUA_Test", LUACallback<int, TestClass *, int>);
	ls2->RegisterFunction("LUA_Test", ff);
	
	START;
	ls2->Run();
	END;
	PRINT_TIME;
	*/
	
	//ANTI_OPTIMALIZATION;

}

#endif