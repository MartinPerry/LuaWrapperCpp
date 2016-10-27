#ifndef _COMPARISON_OPERATORS_H_
#define _COMPARISON_OPERATORS_H_

#include "./MyStringAnsi.h"
#include "./MyConstString.h"

#include <cstring>

//================================================================

//define forward declaration
#define GENERATE_OPERATOR_FORWARD(p1, p2, op) \
		bool operator op(const p1 &str1, const p2 &str2);

#define GENERATE_BODY_EQUALS(p1, p2) \
	inline bool operator ==(const p1 &str1, const p2 &str2){ \
		if (str1.GetLength() != str2.GetLength()) return false; \
		return (strcmp(str1.GetConstString(), str2.GetConstString()) == 0); \
	};


#define GENERATE_BODY_NOT_EQUALS(p1, p2) \
	inline bool operator !=(const p1 &str1, const p2 &str2){ \
		if (str1.GetLength() == str2.GetLength()) return false; \
		return (strcmp(str1.GetConstString(), str2.GetConstString()) != 0); \
	};
	
#define GENERATE_BODY_GREATER(p1, p2, op) \
	inline bool operator op(const p1 &str1, const p2 &str2){ \
		return (strcmp(str1.GetConstString(), str2.GetConstString()) op 0); \
	};

//================================================================

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, == );
GENERATE_OPERATOR_FORWARD(MyConstString, MyStringAnsi, == );
GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyConstString, == );
GENERATE_OPERATOR_FORWARD(MyConstString, MyConstString, == );


GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, != );
GENERATE_OPERATOR_FORWARD(MyConstString, MyStringAnsi, != );
GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyConstString, != );
GENERATE_OPERATOR_FORWARD(MyConstString, MyConstString, != );

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, < );
GENERATE_OPERATOR_FORWARD(MyConstString, MyStringAnsi, < );
GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyConstString, < );
GENERATE_OPERATOR_FORWARD(MyConstString, MyConstString, < );

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, > );
GENERATE_OPERATOR_FORWARD(MyConstString, MyStringAnsi, > );
GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyConstString, > );
GENERATE_OPERATOR_FORWARD(MyConstString, MyConstString, > );

//=====================================================================

GENERATE_BODY_EQUALS(MyStringAnsi, MyStringAnsi);
GENERATE_BODY_EQUALS(MyConstString, MyStringAnsi);
GENERATE_BODY_EQUALS(MyStringAnsi, MyConstString);
GENERATE_BODY_EQUALS(MyConstString, MyConstString);


GENERATE_BODY_NOT_EQUALS(MyStringAnsi, MyStringAnsi);
GENERATE_BODY_NOT_EQUALS(MyConstString, MyStringAnsi);
GENERATE_BODY_NOT_EQUALS(MyStringAnsi, MyConstString);
GENERATE_BODY_NOT_EQUALS(MyConstString, MyConstString);


GENERATE_BODY_GREATER(MyStringAnsi, MyStringAnsi, <);
GENERATE_BODY_GREATER(MyConstString, MyStringAnsi, <);
GENERATE_BODY_GREATER(MyStringAnsi, MyConstString, <);
GENERATE_BODY_GREATER(MyConstString, MyConstString, <);

GENERATE_BODY_GREATER(MyStringAnsi, MyStringAnsi, >);
GENERATE_BODY_GREATER(MyConstString, MyStringAnsi, >);
GENERATE_BODY_GREATER(MyStringAnsi, MyConstString, >);
GENERATE_BODY_GREATER(MyConstString, MyConstString, >);


//==========================================================


bool operator ==(const char * str1, const MyStringAnsi & str2);
bool operator ==(const MyStringAnsi & str1, const char * str2);
bool operator ==(const MyConstString & str1, const char * str2);
bool operator ==(const char * str1, const MyConstString & str2);

bool operator !=(const char * str1, const MyStringAnsi &str2);
bool operator !=(const MyStringAnsi &str1, const char * str2);
bool operator !=(const MyConstString & str1, const char * str2);
bool operator !=(const char * str1, const MyConstString & str2);


inline bool operator ==(const char * str1, const MyStringAnsi & str2)
{
	return (strcmp(str1, str2.GetConstString()) == 0); 
};

inline bool operator ==(const MyStringAnsi &str1, const char * str2)
{
	return (strcmp(str1.GetConstString(), str2) == 0);
};

inline bool operator ==(const char * str1, const MyConstString & str2)
{
	return (strcmp(str1, str2.GetConstString()) == 0);
};

inline bool operator ==(const MyConstString &str1, const char * str2)
{
	return (strcmp(str1.GetConstString(), str2) == 0);
};



inline bool operator !=(const char * str1, const MyStringAnsi &str2)
{
	
	return (strcmp(str1, str2.GetConstString()) != 0);
};

inline bool operator !=(const  MyStringAnsi &str1, const char * str2)
{
	return (strcmp(str1.GetConstString(), str2) != 0);
};


inline bool operator !=(const char * str1, const  MyConstString & str2)
{
	return (strcmp(str1, str2.GetConstString()) != 0);
};

inline bool operator !=(const  MyConstString &str1, const char * str2)
{
	return (strcmp(str1.GetConstString(), str2) != 0);
};



//================================
//undef defined values

#ifdef GENERATE_OPERATOR_FORWRAD
	#undef GENERATE_OPERATOR_FORWRAD
#endif

#ifdef GENERATE_BODY_EQUALS
	#undef GENERATE_BODY_EQUALS
#endif

#ifdef GENERATE_BODY_NOT_EQUALS
	#undef GENERATE_BODY_NOT_EQUALS
#endif

#ifdef GENERATE_BODY_GREATER
	#undef GENERATE_BODY_GREATER
#endif



#endif