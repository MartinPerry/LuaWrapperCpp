#include "./Benchmarks.h"


#define WITH_ATTRIBUTES

//#define LUA_INTF
#define LUA_WRAPPER_CPP

//===============================================================================
//===============================================================================
//===============================================================================

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

extern "C"
{
#include "./LUA/lua_lib/lua.h"
#include "./LUA/lua_lib/lualib.h"
#include "./LUA/lua_lib/lauxlib.h"
}


#include "./TestClass.h"

#include "./LUA/LuaWrapperCpp.h"


#ifdef LUA_INTF
#include <LuaIntf/LuaIntf.h>
#endif



#define START t1 = std::chrono::high_resolution_clock::now()
#define END t2 = std::chrono::high_resolution_clock::now()
#define PRINT_TIME std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl
#define ANTI_OPTIMALIZATION double sum = 0; for (int i = 0; i < COUNT; i++) { sum += res[i]; } std::cout << "Array sum = " << sum << std::endl;
#define RESET_ARRAY for (int i = 0; i < COUNT; i++) { res[i] = 0;}

#define TEST_INIT const int COUNT = 10000000; \
	std::chrono::high_resolution_clock::time_point t1; \
	std::chrono::high_resolution_clock::time_point t2; \
	double * res = new double[COUNT + 1];

#define TEST_END delete[] res;

//===============================================================================
//===============================================================================
//===============================================================================

void LuaWrapperCppCenchmark(const LuaString & name)
{
	std::shared_ptr<Lua::LuaScript> ls = Lua::LuaWrapper::GetInstance()->AddScriptFromFile(name, name);
	

	Lua::LuaClassBind<Account> cb("Account");
	cb.SetDefaultCtor<double>();
	cb.AddCtor<double, double>("Account_2");	
	cb.AddMethod("Print0", CLASS_METHOD_OVERLOAD(BaseAccount, Print0));
	cb.AddMethod("Print0L", CLASS_METHOD_OVERLOAD(BaseAccount, Print0, double, double));	
	cb.AddMethod("Print1", CLASS_METHOD(Account, Print1));
	cb.AddMethod("Print2", CLASS_METHOD(Account, Print2));
	cb.AddMethod("Print3", CLASS_METHOD(Account, Print3));
	cb.AddMethod("Print4", CLASS_METHOD(Account, Print4));
	cb.AddMethod("PrintConst", CLASS_METHOD(Account, PrintConst));	
	cb.AddMethod("Print5", CLASS_METHOD(Account, Print5));
	cb.AddMethod("Print6", CLASS_METHOD(Account, Print6));
	cb.AddMethod("Print7", CLASS_METHOD(Account, Print7));
	cb.AddMethod("deposit", CLASS_METHOD(Account, deposit));
	cb.AddMethod("balance", CLASS_METHOD(Account, balance));
	
#ifdef WITH_ATTRIBUTES
	cb.AddAttribute("vv", CLASS_ATTRIBUTE(BaseAccount, val));
	cb.AddAttribute("xx", CLASS_ATTRIBUTE(Account, xx));
	cb.AddAttribute("cc", CLASS_ATTRIBUTE(Account, cc));
#endif
	
	/*
	cb.toString = [](void * a) -> std::string {
		Account * aa = (Account *)a;

		std::string str = "str";
		str += std::to_string(aa->val);

		return str;
	};
	*/

	ls->RegisterLuaClass(cb);


	srand(time(NULL));

	
	TEST_INIT;


	START;
	ls->Run();
	END;
	PRINT_TIME;

	TEST_END;

	
}

#ifdef LUA_INTF
void LuaIntfBenchmark(const LuaString & name)
{
	


	Lua::LuaScript *ls = Lua::LuaWrapper::GetInstance()->AddScriptFromFile(name, name);
	

	LuaIntf::LuaBinding(ls->GetState()).beginClass<Account>("Account")
		.addConstructor(LUA_ARGS(double))
		//.addFunction("Print0", &Account::Print0, LUA_ARGS())
		//.addFunction("Print0L", &Account::Print0, LUA_ARGS(double, double))
		
		.addFunction("Print1", &Account::Print1, LUA_ARGS(const short&))
		.addFunction("Print2", &Account::Print2, LUA_ARGS(LuaString, int, int))
		.addFunction("Print3", &Account::Print3, LUA_ARGS(const LuaString &, int, int))
		.addFunction("Print4", &Account::Print4, LUA_ARGS())
		.addFunction("PrintConst", &Account::PrintConst, LUA_ARGS(int))
		.addFunction("Print5", &Account::Print5, LUA_ARGS(int))
		.addFunction("Print6", &Account::Print6, LUA_ARGS(Account *))
		.addFunction("Print7", &Account::Print7, LUA_ARGS(Account &))
		.addFunction("deposit", &Account::deposit, LUA_ARGS(double))
		.addFunction("balance", &Account::balance, LUA_ARGS())	
		
#ifdef WITH_ATTRIBUTES
		.addVariable("vv", &Account::val, &Account::val)
		.addVariable("xx", &Account::xx, &Account::xx)
		.addVariable("cc", &Account::cc, &Account::cc)
#endif
		.endClass();
	
	
	TEST_INIT;

	START;
	ls->Run();
	END;
	PRINT_TIME;

	TEST_END;
}
#endif


void CppBenchmark()
{
	TEST_INIT;

	START;
	Account * a = new Account(150);
	for (int i = 1; i <= COUNT; i++)
	{
		a->deposit(i);
	}
	printf("%f\n", a->balance());

#ifdef WITH_ATTRIBUTES
	for (int i = 1; i <= COUNT; i++)
	{
		a->val += i;
	}
	printf("%f\n", a->val);
#endif		

	END;
	PRINT_TIME;

	TEST_END;
}

void RunBenchmarkCppFromLua()
{
	LuaString name = "benchmark.lua";

#ifdef LUA_WRAPPER_CPP
	LuaWrapperCppCenchmark(name);
#endif

#ifdef LUA_INTF
	LuaIntfBenchmark(name);
#endif

	CppBenchmark();
	
}


double testSinCpp(double x)
{
	return std::sin(x);
}

double testSumCpp(double x, double y)
{
	return x + y;
}

void RunBenchmarkLuaFromCpp()
{
	LuaString script = "function testSin(x) return math.sin(x) end\n";
	script += "function testSum(x, y) return x + y end\n";

	std::shared_ptr<Lua::LuaScript> s = Lua::LuaWrapper::GetInstance()->AddScript("test_sin", script);

	Lua::LuaFunction lfSin(s, "testSin");
	Lua::LuaFunction lfSum(s, "testSum");
	
	TEST_INIT;	
	RESET_ARRAY;

	START;

	double x = 0.1;
	for (int i = 1; i <= COUNT; i++)
	{
		res[i] += lfSin.Call<double>(x);
		x += 0.01;
	}

	END;	
	PRINT_TIME;

	
	START;

	x = 0.1;
	for (int i = 1; i <= COUNT; i++)
	{
		res[i] += testSinCpp(x);
		x += 0.01;
	}

	END;
	PRINT_TIME;
	
	//-----------------------------------

	START;

	x = 0.1;
	double y = 0.5;
	for (int i = 1; i <= COUNT; i++)
	{
		res[i] += lfSum.Call<double>(x, y);
		x += 0.01;
		y += i;
	}

	END;
	PRINT_TIME;


	START;

	x = 0.1;
	y = 0.5;
	for (int i = 1; i <= COUNT; i++)
	{
		res[i] += testSumCpp(x, y);
		x += 0.01;
		y += i;
	}

	END;	
	PRINT_TIME;

	ANTI_OPTIMALIZATION;

	TEST_END;
}