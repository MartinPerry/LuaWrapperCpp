# LuaWrapperCpp
Lua Wrapper for C/C++ written from scratch

Binding C++ class to wrapper
------------------------------------------

Class can be binded for use within Lua (for now, only instance methods and public members are supported). 
Each class is wrapped with `LuaClassBind`.

We have simple C++ test class:
````class TestClass
    {
    public:
        double m_val = 148;
		
		TestClass(double val) { m_val = val; }
				
		void Print0() { printf("PRINTF_0\n"); }

		void Print0(double vl, double v2) { printf("PRINTF_0: overload %f %f\n", vl, v2); }

		void Print1(const short& v) { printf("PRINTF_1 %i\n", v); }
        
		int Print2() { printf("PRINTF_2\n"); return 5;}
		
		void Print3(TestClass * t) { printf("PRINTF_3 % f\n", t->m_val);}
		
		void Print4(TestClass & t) { printf("PRINTF_4 % f\n", t.m_val);}
		
    };	
````

And bind it to `LuaClassBind`
````
	Lua::LuaClassBind<TestClass> cb("TestClass");	
	
	cb.AddMethod("Print0", CLASS_METHOD_OVERLOAD(TestClass, Print0));
	cb.AddMethod("Print0_args", CLASS_METHOD_OVERLOAD(TestClass, Print0, double, double));
			
	cb.AddMethod("Print1", CLASS_METHOD(TestClass, Print1));
	cb.AddMethod("Print2", CLASS_METHOD(TestClass, Print2));
	cb.AddMethod("Print3", CLASS_METHOD(TestClass, Print3));
	cb.AddMethod("Print4", CLASS_METHOD(TestClass, Print4));
	
	cb.AddAttribute("m_val", CLASS_ATTRIBUTE(TestClass, m_val));
	
	cb.ctor = [](Lua::LuaScript * script) {
		return new TestClass(script->GetFnInput<double>());
	};

	cb.toString = MyFunction<std::string, TestClass *>([](TestClass * a) -> std::string {
		return "string...";
	});
	
````
`ctor` contains lambda function with constructor calling. 
`toString` is optional and contains lambda for printing method from Lua, when `__tostring` metamethod is called.
`CLASS_METHOD`, `CLASS_ATTRIBUTE` and `CLASS_METHOD_OVERLOAD` are macros to simplify binding. They are defined in `LuaMacros.h`.

And you can call it from Lua as
````
	t = TestClass(150)
	t:Print0()
	t:Print0_args(1, 2)
	local r = t:Print2()
	t:Print3(t)
	t:Print4(t)
	
	t.m_val = 10
	print(t.m_val)
````


References
------------------------------------------

Lua reference manual helper:

* http://pgl.yoyo.org/luai/i/_

Stackoverflow questions related to this project:

* http://stackoverflow.com/questions/40060728/c-make-tupple-from-variadic-template

* http://stackoverflow.com/questions/40224646/c-template-method-specialization-to-return-pointer-or-reference

* http://stackoverflow.com/questions/40135561/decltype-for-overloaded-member-function

* http://stackoverflow.com/questions/40116828/c-stddecayt-and-stdmake-tupled-cannot-convert-argument-from-d-to

* http://stackoverflow.com/questions/40082173/c11-template-stdenable-if-and-stdresult-of

* http://stackoverflow.com/questions/40072627/c-template-parameter-as-function-call-name

* http://stackoverflow.com/questions/40418367/c11-stddeclvalt-name-of-method/40434788#40434788