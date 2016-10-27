#ifndef MY_CONST_STRING_H
#define MY_CONST_STRING_H

#define _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <cstring>


#include "./MyStringMacros.h"

#ifndef ID_DEBUG	
	#include "./MurmurHash3.h"
#endif


class MyStringAnsi;

class MyConstString
{
	public:
		
		#ifdef CONSTEXPR_DISABLE
			MyConstString(const char * key);
		#else
			constexpr MyConstString(const char * key);
		#endif

		MyConstString(const MyConstString& other);		
		MyConstString(const MyStringAnsi& other);
		

		~MyConstString();

				

		const char * GetConstString() const { return this->c_str; };
		int GetLength() const { return this->length; };
		
		
		uint32 GetHashCode() const;
		char GetCharAt(int index) const;

	
		//std::vector<MyStringAnsi> Split(const char * delimeters) const;
		std::vector<double> GetAllNumbers() const;
			
		MyConstString & operator = (const MyConstString & str);
		MyConstString & operator = (const char * str);

	
		operator uint32() const { return GetHashCode(); }



		
	private:
		const char * c_str;
		int length;
		volatile uint32 hashCode;

		MyConstString(char * str);
		//MyConstString(const MyStringAnsi & str);

	

	
};

//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash < MyConstString >
	{
		std::size_t operator()(const MyConstString & k) const
		{
			return k.GetHashCode();
		};
	};	
};


#ifdef CONSTEXPR_DISABLE
inline MyConstString::MyConstString(const char * str)
	: c_str(str),
	length(strlen(str)),
	hashCode(CALC_HASH(str))
{

}
#else
constexpr inline MyConstString::MyConstString(const char * str)
	: c_str(str),
	  length(static_cast<int>(StringLengthCExpr(str))),
	  hashCode(CALC_HASH(str))
{
		
}
#endif

#include "./ComparisonOperators.inl"

#undef _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#undef _CRT_SECURE_NO_DEPRECATE

#endif
