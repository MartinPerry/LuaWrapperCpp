#include "./LuaWrapper.h"
#include "./LuaScript.h"

#include "../Logger.h"


extern "C" 
{	
	#include "./lua_lib/lua.h"
	#include "./lua_lib/lualib.h"
	#include "./lua_lib/lauxlib.h"
} 


using namespace MyUtils;
//using namespace Lua;

LuaWrapper * LuaWrapper::instance = NULL;

LuaWrapper::LuaWrapper()
{
}


LuaWrapper::~LuaWrapper()
{
	this->Release();
}

void LuaWrapper::Release()
{
	std::unordered_map<lua_State *, LuaScript *>::iterator it;
	for (it = this->luaScripts.begin(); it != this->luaScripts.end(); it++)
	{
		SAFE_DELETE(it->second);		
	}
	this->luaScripts.clear();


	std::unordered_map<MyStringAnsi, IFunction *>::iterator jt;
	for (jt = this->registeredFunctions.begin(); jt != this->registeredFunctions.end(); jt++)
	{		
		SAFE_DELETE(jt->second);
	}
	this->registeredFunctions.clear();


	this->globalVariales.clear();
}


void LuaWrapper::Initialize()
{
	if (instance == NULL)
	{
		instance = new LuaWrapper();
	}
}

void LuaWrapper::Destroy()
{
	SAFE_DELETE(instance);
}

LuaWrapper * LuaWrapper::GetInstance()
{
	return instance;
}


bool LuaWrapper::ExistScript(lua_State * state) const
{
	if (this->luaScripts.find(state) == this->luaScripts.end())
	{
		return false;
	}

	return true;
}

LuaScript * LuaWrapper::GetScript(lua_State * state)
{
	if (!this->ExistScript(state))
	{
		return NULL;
	}

	return this->luaScripts[state];
}

IFunction * LuaWrapper::GetMappedFunction(const MyStringAnsi & luaFName)
{
	auto res = this->registeredFunctions.find(luaFName);
	if (res == this->registeredFunctions.end())
	{
		return NULL;
	}
	
	return res->second;
}

void LuaWrapper::RegisterGlobalVariable(const MyStringAnsi & name, void * globalVar)
{
	this->globalVariales.insert(std::make_pair(name, globalVar));
}

LuaScript * LuaWrapper::AddScript(const MyStringAnsi & scriptName, const MyStringAnsi & scriptFileName)
{
	MyStringAnsi script = MyStringAnsi::LoadFromFile(scriptFileName);

	lua_State * state = luaL_newstate();
	//int status = luaL_loadstring( state, script.GetConstString() );
	int status = luaL_loadbuffer(state, script.GetConstString(), script.GetLength(), scriptName.GetConstString());
	if( status )
    {
		MY_LOG_ERROR("Failed to load LUA script %s : %s", scriptName.GetConstString(),
			lua_tostring( state, -1 ));
        
		return NULL;
    }

	lua_setglobal(state, scriptName.GetConstString());
	

	LuaScript * ls = new LuaScript(state, scriptName, scriptFileName);
	
	std::unordered_map<MyStringAnsi, void * >::const_iterator it;
	for (it = this->globalVariales.begin(); it != this->globalVariales.end(); it++)
	{
		ls->SetGlobalVar(it->first, it->second);
	}

	this->luaScripts.insert(std::make_pair(state, ls));

	return ls;
}

void LuaWrapper::ReloadAll()
{
	std::unordered_map<lua_State *, LuaScript *>::iterator it;

	for (it = this->luaScripts.begin(); it != this->luaScripts.end(); it++)
	{
		it->second->Reload();
	}

}