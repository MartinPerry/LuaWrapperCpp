#ifndef MY_STRING_UTILS
#define MY_STRING_UTILS

class MyStringAnsi;
class MyConstString;

class MyStringUtils 
{
	public:		
		static float StringToFloat(const MyStringAnsi & str);
		static bool StringToBool(const MyStringAnsi & str);

		static int StringToInt(const char * str);
		static int StringToInt(const MyStringAnsi & str);
		static int StringToInt(const MyConstString & str);

};

#endif