#include "./LuaWrapper.h"


#include "../Logger.h"

#include "./LuaWrapperCpp.h"


//using namespace MyUtils;
using namespace Lua;

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
	for (auto it = this->luaScripts.begin(); it != this->luaScripts.end(); it++)
	{
		SAFE_DELETE(it->second);		
	}
	this->luaScripts.clear();

	
	
	for (auto it = this->classes.begin(); it != this->classes.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	this->classes.clear();

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

void LuaWrapper::AddClass(const LuaClass & luaClass)
{


	LuaClass * c = new LuaClass(luaClass);
	this->classes[c->ctorName] = c;
}


LuaScript * LuaWrapper::AddScript(const LuaString & scriptName, const LuaString & scriptFileName)
{
	MyStringAnsi script = MyStringAnsi::LoadFromFile(scriptFileName.c_str());

	lua_State * state = luaL_newstate();
	//int status = luaL_loadstring( state, script.GetConstString() );
	int status = luaL_loadbuffer(state, script.c_str(), script.GetLength(), scriptName.c_str());
	if( status )
    {
		MY_LOG_ERROR("Failed to load LUA script %s : %s", scriptName.c_str(),
			lua_tostring( state, -1 ));
        
		return NULL;
    }

	lua_setglobal(state, scriptName.c_str());
	

	LuaScript * ls = new LuaScript(state, scriptName, scriptFileName);
		
	for (auto it = this->classes.begin(); it != this->classes.end(); it++)
	{
		ls->RegisterClass(it->second);
	}

	/*
	std::unordered_map<MyStringAnsi, void * >::const_iterator it;
	for (it = this->globalVariales.begin(); it != this->globalVariales.end(); it++)
	{
		ls->SetGlobalVar(it->first, it->second);
	}
	*/

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