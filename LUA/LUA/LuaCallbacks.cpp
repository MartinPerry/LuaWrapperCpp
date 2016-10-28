#include "./LuaCallbacks.h"

std::unordered_map<MyConstString, std::function<void*(Lua::LuaScript *)> > LuaCallbacks::ctors;
std::unordered_map<MyConstString, std::function<std::string(void *)> > LuaCallbacks::toString;
