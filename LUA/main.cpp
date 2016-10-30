
#define MAIN_MAIN
#ifdef MAIN_MAIN

#include "./LUA/LuaWrapperCpp.h"

#include "FunctionCallback.h"

#include "./Strings/MyString.h"

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



//=================================================================================================
//=================================================================================================
//=================================================================================================


//=================================================================================================
//=================================================================================================
//=================================================================================================


//https://github.com/SteveKChiu/lua-intf



//=================================================================================================
//=================================================================================================
//=================================================================================================

class Account2 {
public:

	Account2(double balance) { m2 = 30; m_balance = balance; }
	void deposit2(double amount) { m_balance += amount + m2; }
	void withdraw2(double amount) { m_balance -= amount; }
	double balance2(void) { return m_balance; }
	void Print0() { printf("Account2 EMPTY\n"); }
private:
	double m_balance;
	double m2;
};

class Account {
public:
	
	double val = 148;
	MyStringAnsi xx = "ahoj aatr";
	Account2 * cc = new Account2(1);

	Account(double balance) { m_balance = balance;; }
	//Account(const Account & c) { m_balance = c.m_balance; printf("CC\n"); };
	//Account(Account && c) {printf("MC\n"); };

	void deposit(double amount) { m_balance += amount; }
	void withdraw(double amount) { m_balance -= amount; }
	double balance(void) 
	{ 
		return m_balance; 
	}
	double balance2(int s) { printf("MUL: %i\n", s); return s * m_balance; }
	void Print0() 
	{ 
		printf("STR0: EMPTY\n"); 
	}
	void Print1(const short& xx) 
	{ 
		printf("STR1: EMPTY %i\n", xx); 	
	}

	void Print2(MyStringAnsi tt, int ii, int jj) 
	{ 
		printf("STR2: %s %i %i\n", tt.c_str(), ii, jj); 
	}

	//void Print3(const MyStringAnsi & tt, int ii, int jj)
	void Print3(const MyStringAnsi & tt, int ii, int jj)
	{
		printf("STR3: %s %i %i\n", tt.c_str(), ii, jj);
	}

	int Print4()
	{
		printf("STR4\n");
		return 5;
	}

	int Print5(int x)
	{
		printf("STR5 %i\n", x);
		return x + 5;
	}

	
	void Print6(Account * c)
	{
		c->m_balance *= -1;
		printf("STR6: %f\n", c->m_balance);
	}

	void Print7(Account & c)
	{
		c.m_balance *=- 1;
		printf("STR7: %f\n", c.m_balance);
	}

private:
	double m_balance;
};



//=============================================================================================
//=============================================================================================
//=============================================================================================



/*
#define method(class, name) {#name, class::name}

const luaL_Reg LuaAccount::methods[] = {
	method(LuaAccount, deposit),
	method(LuaAccount, withdraw),
	method(LuaAccount, balance),
	{ 0,0 }
};
*/



//=================================================================================================
//=================================================================================================
//=================================================================================================

//=================================================================================================
//=================================================================================================
//=================================================================================================

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

//=================================================================================================
//=================================================================================================
//=================================================================================================




//=================================================================================================
//=================================================================================================
//=================================================================================================

using namespace std;


void luaW_printstack(lua_State* L)
{
	int stack = lua_gettop(L);
	for (int i = 1; i <= stack; i++)
	{
		std::cout << std::dec << i << ": " << lua_typename(L, lua_type(L, i));
		switch (lua_type(L, i))
		{
			case LUA_TBOOLEAN: std::cout << " " << lua_toboolean(L, i); break;
			case LUA_TSTRING: std::cout << " " << lua_tostring(L, i); break;
			case LUA_TNUMBER: std::cout << " " << std::dec << (uintptr_t)lua_tointeger(L, i) << " (0x" << std::hex << lua_tointeger(L, i) << ")"; break;
			default: std::cout << " " << std::hex << lua_topointer(L, i); break;
		}
		std::cout << std::endl;
	}
}



//==================================================================================================
//==================================================================================================
//==================================================================================================


//template<typename T, typename ... Args>
//using get_print_type = decltype(std::declval<T>().print(std::declval<Args>() ...)) (T::*)(Args ...);





Lua::LuaScript * Create()
{
	//http://loadcode.blogspot.cz/2007/02/wrapping-c-classes-in-lua.html
	//https://john.nachtimwald.com/2014/07/12/wrapping-a-c-library-in-lua/

	Lua::LuaScript *ls = Lua::LuaWrapper::GetInstance()->AddScript("t2.lua", "t2.lua");
	

	//ls->RegisterFunction("Print_fce", NULL);
	
	Lua::LuaClassBind<Account> cb;
	cb.className = "Account";		
	cb.AddMethod("Print0", CLASS_METHOD(Account, Print0));
	cb.AddMethod("Print1", CLASS_METHOD(Account, Print1));
	cb.AddMethod("Print2", CLASS_METHOD(Account, Print2));
	cb.AddMethod("Print3", CLASS_METHOD(Account, Print3));
	cb.AddMethod("Print4", CLASS_METHOD(Account, Print4));
	cb.AddMethod("Print5", CLASS_METHOD(Account, Print5));
	cb.AddMethod("Print6", CLASS_METHOD(Account, Print6));
	cb.AddMethod("Print7", CLASS_METHOD(Account, Print7));
	cb.AddMethod("deposit", CLASS_METHOD(Account, deposit));
	cb.AddMethod("balance", CLASS_METHOD(Account, balance));
	cb.ctor = [](Lua::LuaScript * script)  {
		return new Account(script->GetFnInput<double>());		
	};

	cb.AddAttribute("vv", CLASS_ATTRIBUTE(Account, val));
	cb.AddAttribute("xx", CLASS_ATTRIBUTE(Account, xx));
	cb.AddAttribute("cc", CLASS_ATTRIBUTE(Account, cc));

	/*
	cb.toString = MyFunction<std::string, Account *>([](Account * a) -> std::string {
		return "string...";
	});
	*/
	/*
	cb.toString = [](void * a) -> std::string {
		Account * aa = (Account *)a;

		std::string str = "str";
		str += std::to_string(aa->val);

		return str;
	};
	*/
	ls->RegisterClass<Account>(cb);


	ls->RegisterFunction("Print_fce", METHOD(HelloMethod));
	ls->RegisterFunction("Print_fce2", METHOD(HelloMethodParam));


	
	Lua::LuaClassBind<Account2> cb2;
	cb2.className = "Account2";
	cb2.AddMethod("PrintX", CLASS_METHOD(Account2, Print0));
	cb2.ctor = [](Lua::LuaScript * script) {
		return new Account2(script->GetFnInput<double>());
	};

	//ls->RegisterClass<Account2>(cb2);
	

	return ls;
}

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

/*
template <class MT>
struct arg_info;

template <class T, class Res, typename M>
struct arg_info<Res T::*MemPtr>
{
	typedef M MemPtr;
	typedef T ClassType;
	typedef Res RetVal;	
};
*/

template<typename T, T t>
void doSomething2()
{
	//(cl->*MemPtr)++;
}


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
	srand(time(NULL));

	const int COUNT = 1000000;
	const int ARR_SIZE = 100000;
	
	chrono::high_resolution_clock::time_point t1;
	chrono::high_resolution_clock::time_point t2;
	double res[ARR_SIZE];


#define START t1 = chrono::high_resolution_clock::now()
#define END t2 = chrono::high_resolution_clock::now()
#define PRINT_TIME cout << "Time: " << std::chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() << " ms" << endl
#define ANTI_OPTIMALIZATION double sum = 0; for (int i = 0; i < ARR_SIZE; i++) { sum += res[i]; } cout << "Array sum = " << sum << endl;
#define RESET_ARRAY for (int i = 0; i < ARR_SIZE; i++) { res[i] = 0;}

	
	Lua::LuaWrapper::Initialize();


	
	Lua::LuaScript *ls = Create();
	lua_State * L = ls->GetState();


	
	//lua_boxpointer(L, aa);
	//(*(void **)(lua_newuserdata(L, sizeof(void *))) = (aa));
	
	//create data in "lua" memory
	//Account *foo = new (lua_newuserdata(L, sizeof(Account))) Account(450);

	//void * luaMem = lua_newuserdata(L, sizeof(void *));
	//luaMem = aa;

	
	Account* ee = new Account(600);	
	ls->SetGlobalVar("ee", ee);

	Account* cc = new Account(900);
	ls->SetGlobalVarLight("cc", cc);

	ls->Run();
	printf("BBB %f BBB\n", cc->balance());
	printf("BBB %f BBB\n", ee->balance());
	return 0;
	/*
	//ls->RegisterFunction("LUA_Test", LUA_Test);		
	ls->RegisterFunction("LUA_Test", LUA_TestClass);
	START;
	ls->Run();
	END;
	PRINT_TIME;
	


	MyUtils::LuaScript *ls2 = MyUtils::LuaWrapper::GetInstance()->AddScript("t.lua", "t.lua");
	
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