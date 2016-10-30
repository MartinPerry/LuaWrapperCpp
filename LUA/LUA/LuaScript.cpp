#include "./LuaScript.h"



#include "../Logger.h"



using namespace Lua;

//http://pgl.yoyo.org/luai/i/lua_topointer
//http://rubenlaguna.com/wp/2012/11/26/first-steps-lua-cplusplus-integration/
//http://stackoverflow.com/questions/17570145/lua-adding-changing-global-variables-from-c

//http://www.gamedev.net/topic/362520-struct-in-lua/
//http://www.gamedev.net/page/resources/_/technical/game-programming/the-lua-tutorial-r2999
//http://stackoverflow.com/questions/15396067/lua-newbie-c-lua-how-to-pass-a-struct-buffer-to-lua-from-c


LuaScript::LuaScript(lua_State * state, const MyStringAnsi & scriptName, const MyStringAnsi & scriptFileName)
{
	this->state = state;
	this->scriptName = scriptName;
	this->scriptFileName = scriptFileName;

	luaL_openlibs( this->state );

	this->runCount = 0;

	
	//this->GetAllSavedVariables();

}

LuaScript::~LuaScript()
{
	lua_close(this->state);	
}


void LuaScript::Reload()
{
	MyStringAnsi script = MyStringAnsi::LoadFromFile(this->scriptFileName);

	int status = luaL_loadbuffer(this->state, script.GetConstString(), script.GetLength(), this->scriptName.GetConstString());
	if (status)
	{
		MY_LOG_ERROR("Failed to load LUA script %s : %s", this->scriptName.GetConstString(),
			lua_tostring(state, -1));

		return;
	}

	lua_setglobal(this->state, this->scriptName.GetConstString());

}

lua_State * LuaScript::GetState()
{
	return this->state;
}

/*-----------------------------------------------------------
Function:	IsStackEmpty
Returns:
	true - empty, false - not empty
	
Test if LUA stack is empty
-------------------------------------------------------------*/
bool LuaScript::IsStackEmpty() const
{
	if (lua_gettop(this->state) == 0) 
	{
		return true;
	}

	return false;
}

/*-----------------------------------------------------------
Function:	GetStackSize
Returns:
	size of stack
	
Get LUA stack size
-------------------------------------------------------------*/
int LuaScript::GetStackSize() const
{
	return lua_gettop(this->state);
}

/*-----------------------------------------------------------
Function:	GetFnReturnValueCount
Returns:
	number of return values from C function

Get number of return values from C function called from LUA
-------------------------------------------------------------*/
int LuaScript::GetFnReturnValueCount() const
{
	return this->returnValCount;
}

/*-----------------------------------------------------------
Function:	RegisterFunction
Parametrs:
	[in] luaName - name of function in LUA
	[in] fn - pointer to C function to be registered
	
Register new C function to LUA
-------------------------------------------------------------*/
void LuaScript::RegisterFunction(const MyStringAnsi & luaFName, lua_CFunction fn)
{	
	lua_register( this->state, luaFName.c_str(), fn );
}

std::vector<MyStringAnsi> LuaScript::GetAllGlobals()
{
	std::vector<MyStringAnsi> v;
	//Get all global variables from Lua
	lua_pushglobaltable(this->state);       // Get global table
	lua_pushnil(this->state);               // put a nil key on stack
	while (lua_next(this->state,-2) != 0)   // key(-1) is replaced by the next key(-1) in table(-2)
	{ 
		MyStringAnsi name = lua_tostring(this->state,-2);  // Get key(-2) name
		v.push_back(name);

		lua_pop(this->state,1);               // remove value(-1), now key on top at(-1)
	}
	lua_pop(this->state,1);                 // remove global table(-1)
	
	return v;

}



/*-----------------------------------------------------------
Function:	Run

Run script
- stackPtr = 1 
- returnValCount = 0 
-------------------------------------------------------------*/
void LuaScript::Run()
{
	this->stackPtr = 1;
	this->returnValCount = 0;
	
	//http://stackoverflow.com/questions/6434610/how-can-i-know-return-value-count-of-a-lua-function-from-c
	//http://stackoverflow.com/questions/16691486/how-to-decide-using-lua-call-or-lua-pcall
	//http://stackoverflow.com/questions/20938099/optimizing-lua-for-cyclic-execution

	int status = 0;

	lua_getglobal(this->state, "main");
	if(lua_isfunction(this->state, -1) == false)
    {	
		lua_pop(this->state, 1);
		lua_getglobal(this->state, this->scriptName.GetConstString());		

		status = lua_pcall( this->state, 0, LUA_MULTRET, 0 );
			
	}
	else 
	{		
		status = lua_pcall( this->state, 0, LUA_MULTRET, 0 );
	}

	if (status != 0)
	{
		MY_LOG_ERROR("Failed to run LUA script %s : %s", this->scriptName.GetConstString(),
			lua_tostring( this->state, -1 ));
        
	}

	this->runCount++;
}

/*-----------------------------------------------------------
Function:	Reset

Reset inner variables:
- stackPtr = 1 
- returnValCount = 0 
-------------------------------------------------------------*/
void LuaScript::Reset()
{
	this->stackPtr = 1;
	this->returnValCount = 0;
}


void LuaScript::IncStack()
{
	this->stackPtr++;	
}

void LuaScript::PrintStack()
{
	this->PrintStack("");
}

void LuaScript::PrintStack(const MyStringAnsi & id) 
{
	int i = lua_gettop(this->state);
	printf("\n ----------------  Stack Dump [%s] ----------------\n", id.c_str());
	while (i) {
		int t = lua_type(this->state, i);
		switch (t) {
		case LUA_TSTRING:
			printf("%d:'%s'\n", i, lua_tostring(this->state, i));
			break;
		case LUA_TBOOLEAN:
			printf("%d: %s\n", i, lua_toboolean(this->state, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("%d: %g\n", i, lua_tonumber(this->state, i));
			break;				
		default: printf("%d: %s (0x%p)\n", i, lua_typename(this->state, t), lua_topointer(this->state, i)); break;
		}
		i--;
	}
	printf("--------------- Stack Dump Finished [%s] ---------------\n\n", id.c_str());
}

/*-----------------------------------------------------------
Function:	GetFnInputTableInt
Parametrs:
	[in] varName - "table variable" name
Returns:
	int value

Get value from LUA "table". Suply struct
struct X 
{
 int a; - variable name
}
-------------------------------------------------------------*/
int LuaScript::GetFnInputTableInt(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	int val = this->GetReturnValue<int>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up
	return val;
}


/*-----------------------------------------------------------
Function:	GetFnInputTableBool
Parametrs:
	[in] varName - "table variable" name
Returns:
	bool  value

Get value from LUA "table". Suply struct
struct X 
{
 bool a; - variable name
}
-------------------------------------------------------------*/
bool LuaScript::GetFnInputTableBool(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	bool val = this->GetReturnValue<bool>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up
	return val;
}


/*-----------------------------------------------------------
Function:	GetFnInputTableString
Parametrs:
	[in] varName - "table variable" name
Returns:
	MyStringAnsi  value

Get value from LUA "table". Suply struct
struct X 
{
 MyStringAnsi a; - variable name
}
-------------------------------------------------------------*/
MyStringAnsi LuaScript::GetFnInputTableString(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	MyStringAnsi val = this->GetReturnValue<MyStringAnsi>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up

	return val;
}

/*-----------------------------------------------------------
Function:	GetFnInputTableFloat
Parametrs:
	[in] varName - "table variable" name
Returns:
	float  value

Get value from LUA "table". Suply struct
struct X 
{
 float a; - variable name
}
-------------------------------------------------------------*/
float LuaScript::GetFnInputTableFloat(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	float val = this->GetReturnValue<float>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up

	return val;
}

/*-----------------------------------------------------------
Function:	GetFnInputTableDouble
Parametrs:
	[in] varName - "table variable" name
Returns:
	double  value

Get value from LUA "table". Suply struct
struct X 
{
 double a; - variable name
}
-------------------------------------------------------------*/
double LuaScript::GetFnInputTableDouble(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	double val = this->GetReturnValue<double>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up

	return val;
}

/*-----------------------------------------------------------
Function:	GetFnInputTableUnsigned
Parametrs:
	[in] varName - "table variable" name
Returns:
	uint32  value

Get value from LUA "table". Suply struct
struct X 
{
 uint32 a; - variable name
}
-------------------------------------------------------------*/
uint32 LuaScript::GetFnInputTableUnsigned(const MyStringAnsi & varName)
{
	lua_pushstring(this->state, varName.GetConstString()); // let Lua know what we're looking for
	lua_gettable(this->state, this->stackPtr); // let Lua know where we're looking for it
	uint32 val = this->GetReturnValue<uint32>(); // get requested value (which was placed on top of the stack by lua_gettable)
	lua_pop(this->state, 1); // clean up

	return val;
}














