#include "./MyStringUtils.h"

#include <locale.h>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cfloat>

#include "./MyStringAnsi.h"
#include "./MyConstString.h"


#ifndef UINT32_MAX
	#define UINT32_MAX       0xffffffff
#endif

int MyStringUtils::StringToInt(const MyStringAnsi & str)
{	
	return MyStringUtils::StringToInt(str.GetConstString());
}

int MyStringUtils::StringToInt(const MyConstString & str)
{
	return MyStringUtils::StringToInt(str.GetConstString());
}

int MyStringUtils::StringToInt(const char * str)
{
	if (strcmp(str, "MAX") == 0)
	{
		return INT_MAX;
	}
	if (strcmp(str, "MIN") == 0)
	{
		return INT_MIN;
	}
	if (strcmp(str, "INT_MAX") == 0)
	{
		return INT_MAX;
	}
	if (strcmp(str, "INT_MIN") == 0)
	{
		return INT_MIN;
	}


	int value = atoi(str);

	return value;
}

float MyStringUtils::StringToFloat(const MyStringAnsi & str)
{
	if (str.Compare("MAX") == 0)
	{
		return FLT_MAX;
	}
	if (str.Compare("MIN") == 0)
	{
		return FLT_MIN;
	}
	if (str.Compare("FLT_MAX") == 0)
	{
		return FLT_MAX;
	}	
	if (str.Compare("FLT_MIN") == 0)
	{
		return FLT_MIN;
	}
	
	if (str.Compare("AUTO") == 0)
	{
		return FLT_MAX;
	}

	setlocale(LC_NUMERIC, "C");
	//value = static_cast<float>(atof(str.GetConstString()));
	float value = static_cast<float>(strtod(str.GetConstString(), NULL));

	return value;
}

bool MyStringUtils::StringToBool(const MyStringAnsi & str)
{
	if (str.Compare("true") == 0)
	{
		return true;
	}
	if (str.Compare("false") == 0)
	{
		return false;
	}

	if (str.Compare("1") == 0)
	{
		return true;
	}

	return false;
}
