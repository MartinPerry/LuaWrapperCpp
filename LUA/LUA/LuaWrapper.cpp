#include "./LuaWrapper.h"



#include "./LuaWrapperCpp.h"


using namespace Lua;

LuaWrapper * LuaWrapper::instance = NULL;

LuaWrapper::LuaWrapper()
	: scriptLoaderCallback(nullptr)
{
}


LuaWrapper::~LuaWrapper()
{
	this->Release();
}

void LuaWrapper::Release()
{	
	
	this->luaScripts.clear();

	
	
	for (auto it = this->classes.begin(); it != this->classes.end(); it++)
	{
		LUA_SAFE_DELETE(it->second);
	}
	this->classes.clear();

}


void LuaWrapper::Initialize(ScriptLoaderCallback loaderCallback)
{
	if (instance == NULL)
	{
		instance = new LuaWrapper();
		instance->scriptLoaderCallback = loaderCallback;
	}
}

void LuaWrapper::Destroy()
{
	LUA_SAFE_DELETE(instance);
}

LuaWrapper * LuaWrapper::GetInstance()
{
	return instance;
}

void LuaWrapper::AddRegisterCallback(RegisterCallback regCallback)
{
	this->regCallbacks.push_back(regCallback);
}

bool LuaWrapper::ExistScript(lua_State * state) const
{
	if (this->luaScripts.find(state) == this->luaScripts.end())
	{
		return false;
	}

	return true;
}

std::shared_ptr<LuaScript> LuaWrapper::GetScript(lua_State * state)
{
	auto it = this->luaScripts.find(state);
	if (it == this->luaScripts.end())
	{
		return NULL;
	}

	return it->second;
}

void LuaWrapper::AddClass(const LuaClass & luaClass)
{


	LuaClass * c = new LuaClass(luaClass);
	this->classes[c->ctorName] = c;
}

LuaString LuaWrapper::GetScriptFromFile(const LuaString & scriptFileName)
{
	if (this->scriptLoaderCallback == nullptr)
	{
		return LuaUtils::LoadFromFile(scriptFileName);
	}

	return this->scriptLoaderCallback(scriptFileName);
}

std::shared_ptr<LuaScript> LuaWrapper::AddScript(const LuaString & scriptFileName)
{
	return this->AddScript(scriptFileName, scriptFileName);
}

std::shared_ptr<LuaScript> LuaWrapper::AddScript(const LuaString & scriptName, const LuaString & scriptFileName)
{
	LuaString script = this->GetScriptFromFile(scriptFileName);

	lua_State * state = luaL_newstate();
	//int status = luaL_loadstring( state, script.GetConstString() );
	int status = luaL_loadbuffer(state, script.c_str(), script.length(), scriptName.c_str());
	if( status )
    {
		LUA_LOG_ERROR("Failed to load LUA script %s : %s", scriptName.c_str(),
			lua_tostring( state, -1 ));
        
		return NULL;
    }

	lua_setglobal(state, scriptName.c_str());
	

	std::shared_ptr<LuaScript> ls = std::shared_ptr<LuaScript>(new LuaScript(state, scriptName, scriptFileName));
		
	for (auto it = this->classes.begin(); it != this->classes.end(); it++)
	{
		ls->RegisterLuaClass(*(it->second));
	}

	for (size_t i = 0; i < this->regCallbacks.size(); i++)
	{
		this->regCallbacks[i](ls);
	}
	

	this->luaScripts.insert(std::make_pair(state, ls));

	return ls;
}

