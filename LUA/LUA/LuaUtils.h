#ifndef LUA_UTILS_H
#define LUA_UTILS_H

#include <cstdio>
#include <cstring>

#include "./LuaMacros.h"
#include "./LuaTypes.h"

//MSVC
#ifdef _MSC_VER
#ifndef my_fopen 
	#define my_fopen(a, b, c) fopen_s(a, b, c)	
#endif
#ifndef my_strncpy 
	#define my_strncpy(a, b, c, d) strncpy_s(a, b, c, d)
#endif
//Unix
#else
#ifndef my_fopen 
	#define my_fopen(a, b, c) (*a = fopen(b, c))
#endif
#ifndef my_strncpy 
	#define my_strncpy(a, b, c, d) strncpy(a, c, d)
#endif
#endif



namespace Lua
{
	struct LuaUtils 
	{
		static const char * CopyString(const LuaString & str)
		{
			char * strCopy = new char[str.length() + 1];
			my_strncpy(strCopy, str.length() + 1, str.c_str(), str.length());
			strCopy[str.length()] = 0;

			return strCopy;			
		};

		static LuaString LoadFromFile(const LuaString & filePath)
		{
			FILE *f = NULL;  //pointer to file we will read in
			my_fopen(&f, filePath.c_str(), "rb");
			if (f == NULL)
			{
				printf("Failed to open file: \"%s\"\n", filePath.c_str());
				return "";
			}

			fseek(f, 0L, SEEK_END);
			long size = ftell(f);
			fseek(f, 0L, SEEK_SET);

			char * data = new char[size + 1];
			fread(data, sizeof(char), size, f);
			fclose(f);

			data[size] = 0;
			LuaString tmp = LuaString(data);
			LUA_SAFE_DELETE_ARRAY(data);

			return tmp;
		};
	};

}

#endif
