# LuaWrapperCpp
Lua Wrapper for C/C++ written from scratch

This wrapper is still work-in-progress.


Wrapper logic
------------------------------------------
Wrapper is writtent from scratch using C++11 without the in-depth knowledge of existing wrappers. 
If there is a similarity with an existing solution, its purely coincidental.

The main part of the wrapper consist of two classes located in the files with the same name - `LuaWrapper` and `LuaScript`.

`LuaWrapper` is a singleton class. It initialize Lua virtual machine, create new scripts (`LuaScript`) and store them in a map. 
In Lua callbacks, this is used to get `LuaScript` instances from `lua_State *`.

`LuaScript` represents a single script that can be started.

Description of other files:

`LuaWrapperCpp.h` is the main file to include the wrapper.

`LuaCallbacks.h` contains main part of the wrapper. It contains templated meethods that are called from Lua,  methods binded
to Lua metamethods like `__gc`, `__index`, `__newindex`, `__tostring` etc. and constructors for Lua classes.

`LuaFunctionWrapper.h` contains a static class wrapping Lua methods with a templated methods. This way, you can call "generic"
`get` / `set` methods and in background, correct Lua methods are called (`lua_tounsigned`, `lua_tonumber` etc.).

`LuaClassBind.h` contains a helper class for binding C++ class to Lua. Its content is used to build C++ class representation that
can be directly binded to Lua callbacks.

`LuaUtils.h` contains a helper class with static helper methods.

`LuaTypes.h` contains helper typedefs

`LuaMacros.h` contains helper macros to reduce size of the code. Some of these macros can be user-defined to change default behaviour.

How to use it
------------------------------------------
You just have to include `LuaWrapperCpp.h`. 
There are no external dependencies, except Lua library (version 5.2 is included in the project)

If you want, you may change some settings in `LuaMacros.h`

| #define name      | meaning     |  default value |
| ------------------|:-----------:|---------------:|
| `SAFE_PTR_CHECKS` | Safe checks for pointers during retrieval of raw pointers from Lua (see Benchmarks) | 1             |
| `LUA_STRING`      | String class used for LuaStrings. Must have `.c_str()` and `.length()` methods      | `std::string` |


First, you have to start with `LuaWrapper` initialization, which is a singleton class:

````c++
Lua::LuaWrapper::Initialize([](const LuaString & filePath) -> LuaString {
		return Lua::LuaUtils::LoadFromFile(filePath);
	});	
````

You must provide script loading functor. 
This is usefull, if you have a virtual file system and you need to handle loading in a different way.
Function must return loaded string with a script.
You can also pass `nullptr`, in that case default loading `Lua::LuaUtils::LoadFromFile(filePath)` will be used.


In some cases, you may want to add some global variables or register classes / functions for all scripts, that are loaded
with the wrapper.
You can do this using `SetRegisterCallback` that is called after the script has been created via `AddScript`.

````c++
Lua::LuaWrapper::GetInstance()->SetRegisterCallback(
	[&](std::shared_ptr<Lua::LuaScript> script) -> void {
		script->SetGlobalVarClass<Account>("global_ucet", someAccount);
	});	
````			


Binding C++ class to wrapper
------------------------------------------

Class can be binded for use within Lua (for now, only instance methods and public members are supported). 
Each class is wrapped with `LuaClassBind`.

We have simple C++ test class:
````c++
	struct TestStruct 
	{
		float m_x;
		float m_y;
	};

	class TestClass
    {
    public:
        double m_val = 148;
		TestStruct m_tc;
		
		TestClass(double val) { m_val = val; }
		TestClass(double v1, double v2) { m_val = v1 * v2; }
				
		void Print0() { printf("PRINTF_0\n"); }

		void Print0(double vl, double v2) { printf("PRINTF_0: overload %f %f\n", vl, v2); }

		void Print1(const short& v) { printf("PRINTF_1 %i\n", v); }
        
		int Print2() { printf("PRINTF_2\n"); return 5;}
		
		void Print3(TestClass * t) { printf("PRINTF_3 % f\n", t->m_val);}
		
		void Print4(TestClass & t) { printf("PRINTF_4 % f\n", t.m_val);}
		
		void Print5() const { printf("PRINTF_5\n"); }
    };	
````

And bind it to `LuaClassBind`
````c++

	Lua::LuaClassBind<TestStruct> sb("TestStruct");	
	sb.SetDefaultCtor<>();
	
	sb.AddAttribute("m_x", CLASS_ATTRIBUTE(TestClass, m_x));
	sb.AddAttribute("m_y", CLASS_ATTRIBUTE(TestClass, m_y));
	
	sb.SetToString([](TestStruct * a) -> LuaString {		
		return "TestStruct...";
	});
	
	Lua::LuaClassBind<TestClass> cb("TestClass");	
	cb.SetDefaultCtor<double>();
	cb.AddCtor<double, double>("TestClass_ctor2");
	
	cb.AddMethod("Print0", CLASS_METHOD_OVERLOAD(TestClass, Print0));
	cb.AddMethod("Print0_args", CLASS_METHOD_OVERLOAD(TestClass, Print0, double, double));
			
	cb.AddMethod("Print1", CLASS_METHOD(TestClass, Print1));
	cb.AddMethod("Print2", CLASS_METHOD(TestClass, Print2));
	cb.AddMethod("Print3", CLASS_METHOD(TestClass, Print3));
	cb.AddMethod("Print4", CLASS_METHOD(TestClass, Print4));
	cb.AddMethod("Print5", CLASS_METHOD(TestClass, Print5));
	
	cb.AddAttribute("m_val", CLASS_ATTRIBUTE(TestClass, m_val));
	cb.AddAttribute("m_tc", CLASS_ATTRIBUTE(TestClass, m_tc));
	
	cb.SetToString([](TestClass * a) -> LuaString {		
		return "TestClass...";
	});
	
		
````
Each class must have one binded ctor via `SetDefaultCtor<...>`. This ctor name is same as the name of the "class". 
However, you can specifiy additional ctors via `AddCtor<...>` and for each of them, unique name must be set.  
All ctors behave like Factory pattern. In Lua, you call an ordinary method that returns a new class instance.


Method `SetToString` is optional and sets lambda function that is called, when `__tostring` metamethod is called in Lua.
`CLASS_METHOD`, `CLASS_ATTRIBUTE` and `CLASS_METHOD_OVERLOAD` are macros to simplify binding. They are defined in `LuaMacros.h`.

The above binded class can be called from Lua as
````lua
	t = TestClass(150)
	t:Print0()
	t:Print0_args(1, 2)
	local r = t:Print2()
	t:Print3(t)
	t:Print4(t)
	t:Print5()
	
	
	v = TestStruct();
	v.m_x = 15.0
	print(v)
	
	t.m_tc = v
	print(t.m_tc)

	
	t.m_val = 10
	print(t.m_val)
	
	t2 = TestClass_ctor2(150, 150)
	
````

Benchmark & comparison
------------------------------------------
We have compared our solution with [lua-intf](https://github.com/SteveKChiu/lua-intf)


Our solution is compared in two modes: X (`SAFE_PTR_CHECKS 1`) / Y (`SAFE_PTR_CHECKS 0`)
`SAFE_PTR_CHECKS` macro is located in `LuaMacros.h`

````c++
class Account {
public:
	double val = 148;
	Account(double balance) { m_balance = balance; }
	void deposit(double amount) { m_balance += amount; }
	double balance(void) { return m_balance; }
}
````

Test #1
Only class-methods exposed to Lua

````lua
a = Account(150)

for i=1, 10000000 do
	a:deposit(i)
end

print(a:balance())
````

| Wrapper       | Time [ms]     |
| ------------- |:-------------:|
| Ours          | 927 / 605  |
| Lua-intf      | 1183 |


Test #2
Class-members exposed to Lua

````lua
a = Account(150)

for i=1, 10000000 do
	a.val = a.val + i
end

print(a.val)
````

| Wrapper       | Time [ms]     |
| ------------- |:-------------:|
| Ours          | 2688 / 2133  |
| Lua-intf      | 4305 |


Test #3
Class-methods and members exposed to Lua

````lua
a = Account(150)

for i=1, 10000000 do
	a:deposit(i)
end

print(a:balance())

for i=1, 10000000 do
	a.val = a.val + i
end

print(a.val)
````

| Wrapper       | Time [ms]     |
| ------------- |:-------------:|
| Ours          | 4413 / 3241 |
| Lua-intf      | 5467 |


Test #4
Class-methods and members exposed to Lua

````lua
a = Account(150)

for i=1, 10000000 do
	a:deposit(i)
end

print(a:balance())
````

| Wrapper       | Time [ms]     |
| ------------- |:-------------:|
| Ours          | 1511 / 1172 |
| Lua-intf      | 1112 |


As you can see from above test, our wrapper in "unsafe" mode (SAFE_PTR_CHECKS 0) is faster, however, you are loosing type control.
This solution is recomended for release builds and only if scripts have already been tested and are working.

If we bind class-methods and members together, but members are not used, our wrapper is slower (see Test #4). 
That is because each method call goes through `__index` metamethod and it slows thing down a bit. 
We should inspect Lua code before actual binding and see, if members are used and based on this disable `__index` call. 
However, this is in our TO-DO list and right now, other things are more important.
So, if you are not using class-members in your code, do not bind them to Lua.

References
------------------------------------------

Lua reference manual helper:

* http://pgl.yoyo.org/luai/i/_

Stackoverflow questions related to this project:

Many thanks to those, who helped. Without them, it would not be possible to create this

* http://stackoverflow.com/questions/40060728/c-make-tupple-from-variadic-template

* http://stackoverflow.com/questions/40224646/c-template-method-specialization-to-return-pointer-or-reference

* http://stackoverflow.com/questions/40135561/decltype-for-overloaded-member-function

* http://stackoverflow.com/questions/40116828/c-stddecayt-and-stdmake-tupled-cannot-convert-argument-from-d-to

* http://stackoverflow.com/questions/40082173/c11-template-stdenable-if-and-stdresult-of

* http://stackoverflow.com/questions/40072627/c-template-parameter-as-function-call-name

* http://stackoverflow.com/questions/40418367/c11-stddeclvalt-name-of-method

* http://stackoverflow.com/questions/40436966/c-const-vs-non-const-member-function-template-with-function-pointers