#include "./LuaScript.h"

extern "C"
{
	#include "./lua_lib/lua.h"
	#include "./lua_lib/lualib.h"
	#include "./lua_lib/lauxlib.h"
}


#include "./LuaCallbacks.h"


using namespace Lua;

//http://pgl.yoyo.org/luai/i/lua_topointer
//http://rubenlaguna.com/wp/2012/11/26/first-steps-lua-cplusplus-integration/
//http://stackoverflow.com/questions/17570145/lua-adding-changing-global-variables-from-c

//http://www.gamedev.net/topic/362520-struct-in-lua/
//http://www.gamedev.net/page/resources/_/technical/game-programming/the-lua-tutorial-r2999
//http://stackoverflow.com/questions/15396067/lua-newbie-c-lua-how-to-pass-a-struct-buffer-to-lua-from-c


LuaScript::LuaScript(lua_State * state, const LuaString & scriptName, const LuaString & scriptFileName)
{
	this->state = state;
	this->scriptName = scriptName;
	this->scriptFileName = scriptFileName;

	luaL_openlibs( this->state );

	this->runCount = 0;

	this->returnLightUserData = false;	
}

LuaScript::~LuaScript()
{
	lua_close(this->state);	
}

/*
void LuaScript::Reload()
{
	LuaString script = LuaUtils::LoadFromFile(this->scriptFileName);

	int status = luaL_loadbuffer(this->state, script.c_str(), script.length(), this->scriptName.c_str());
	if (status)
	{
		LUA_LOG_ERROR("Failed to load LUA script %s : %s", this->scriptName.c_str(),
			lua_tostring(state, -1));

		return;
	}

	lua_setglobal(this->state, this->scriptName.c_str());

}
*/

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
Parameters:
	[in] luaName - name of function in LUA
	[in] fn - pointer to C function to be registered
	
Register new C function to LUA
-------------------------------------------------------------*/
void LuaScript::RegisterFunction(const LuaString & luaFName, lua_CFunction fn)
{	
	lua_register( this->state, luaFName.c_str(), fn );
}

/*-----------------------------------------------------------
Function:	RegisterClass
Parameters:
	[in] classBind - binded class

Register new Lua binding for class
-------------------------------------------------------------*/
void LuaScript::RegisterClass(const LuaClass & classBind)
{
	std::type_index key = classBind.typeIndex;

	if (LuaCallbacks::tableName.find(key) != LuaCallbacks::tableName.end())
	{
		LUA_LOG_ERROR("Class %s already registered - std::type_index already exist",
			classBind.ctorName.c_str());
		return;
	}

	const char * classTableName = classBind.ctorName.c_str();

	
	//LuaCallbacks::ctors[key] = classBind.ctor;
	LuaCallbacks::toString[key] = classBind.toString;
	LuaCallbacks::tableName[key] = classTableName;
	

	//http://cfc.kizzx2.com/index.php/binding-c-classes-to-lua-a-step-by-step-example-for-beginners/
	//http://lua-users.org/lists/lua-l/2006-08/msg00245.html


	lua_State * L = this->state;


	luaL_newmetatable(L, classTableName); //mt2
	int metaTableID = lua_gettop(L);


	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");

	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__newindex");


	//Default ctor - named same as "class"
	lua_pushcclosure(L, classBind.create_new, 0);
	lua_setglobal(L, classBind.ctorName.c_str()); // this is how function will be named in Lua

	//other ctors named as specified by user
	for (size_t i = 0; i < classBind.ctors.size() - 1; i++)
	{
		lua_pushcclosure(L, classBind.ctors[i].func, 0);
		lua_setglobal(L, classBind.ctors[i].name); // this is how function will be named in Lua
	}

												  // Register the C functions _into_ the metatable we just created.	
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classTableName);
	luaL_setfuncs(L, &classBind.methods[0], 1);
#else
	luaL_setfuncs(L, &classBind.methods[0], 0);
#endif

	//double underscore variables
	//http://nova-fusion.com/2011/06/30/lua-metatables-tutorial/
	//http://stackoverflow.com/questions/10891957/difference-between-tables-and-metatables-in-lua


	//rewrite __gc with our own
	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, classBind.garbage_collect);
	lua_rawset(L, metaTableID);



	lua_pushliteral(L, "__tostring");
#if (SAFE_PTR_CHECKS == 1)
	lua_pushstring(L, classTableName);
	lua_pushcclosure(L, classBind.to_string, 1);
#else
	lua_pushcclosure(L, classBind.to_string, 0);
#endif
	lua_rawset(L, metaTableID);


	if (classBind.attrs.size() > 1)
	{
		lua_pushliteral(L, "__index");
		lua_pushstring(L, classTableName);
		lua_pushcclosure(L, classBind.index, 1);
		lua_rawset(L, metaTableID);


		lua_pushliteral(L, "__newindex");
		lua_pushstring(L, classTableName);
		lua_pushcclosure(L, classBind.new_index, 1);
		lua_rawset(L, metaTableID);

		for (size_t i = 0; i < classBind.attrs.size() - 1; i++)
		{
			lua_pushstring(L, classBind.attrs[i].name);
			lua_pushlightuserdata(L, classBind.attrs[i].func);
			lua_rawset(L, -3);
		}
	}

					
}




std::vector<LuaString> LuaScript::GetAllGlobals()
{
	std::vector<LuaString> v;
	//Get all global variables from Lua
	lua_pushglobaltable(this->state);       // Get global table
	lua_pushnil(this->state);               // put a nil key on stack
	while (lua_next(this->state,-2) != 0)   // key(-1) is replaced by the next key(-1) in table(-2)
	{ 
		LuaString name = lua_tostring(this->state,-2);  // Get key(-2) name
		v.push_back(name);

		lua_pop(this->state,1);               // remove value(-1), now key on top at(-1)
	}
	lua_pop(this->state,1);                 // remove global table(-1)
	
	return v;

}



/*-----------------------------------------------------------
Function:	Run

Run script
- returnValCount = 0 
-------------------------------------------------------------*/
void LuaScript::Run()
{	
	this->returnValCount = 0;
	
	//http://stackoverflow.com/questions/6434610/how-can-i-know-return-value-count-of-a-lua-function-from-c
	//http://stackoverflow.com/questions/16691486/how-to-decide-using-lua-call-or-lua-pcall
	//http://stackoverflow.com/questions/20938099/optimizing-lua-for-cyclic-execution

	int status = 0;

	lua_getglobal(this->state, "main");
	if(lua_isfunction(this->state, -1) == false)
    {	
		lua_pop(this->state, 1);
		lua_getglobal(this->state, this->scriptName.c_str());

		status = lua_pcall( this->state, 0, LUA_MULTRET, 0 );
			
	}
	else 
	{		
		status = lua_pcall( this->state, 0, LUA_MULTRET, 0 );
	}

	if (status != 0)
	{
		LUA_LOG_ERROR("Failed to run LUA script %s : %s", this->scriptName.c_str(),
			lua_tostring( this->state, -1 ));
        
	}

	this->runCount++;
}

/*-----------------------------------------------------------
Function:	Reset

Reset inner variables:
- returnValCount = 0 
-------------------------------------------------------------*/
void LuaScript::Reset()
{
	this->returnValCount = 0;
}


/*-----------------------------------------------------------
Function:	PrintStack

Print Lua stack
-------------------------------------------------------------*/
void LuaScript::PrintStack()
{
	this->PrintStack("");
}

/*-----------------------------------------------------------
Function:	PrintStack
Parameters:
	[in] id - ID of the stack

Print Lua stack with an ID
-------------------------------------------------------------*/
void LuaScript::PrintStack(const LuaString & id)
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












//=============================================================================
//===================== Set LUA global variable ===============================
//=============================================================================



LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, bool val)
{
	lua_pushboolean(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, float val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, double val)
{
	lua_pushnumber(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, const char * val)
{
	lua_pushstring(this->state, val);
	lua_setglobal(this->state, varName.c_str());
};

LUA_INLINE void LuaScript::SetGlobalVar(const LuaString & varName, const LuaString & val)
{
	lua_pushstring(this->state, val.c_str());
	lua_setglobal(this->state, varName.c_str());
};







