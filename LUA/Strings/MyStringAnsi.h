#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H

#define _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#define _CRT_SECURE_NO_DEPRECATE


#include <wchar.h>
#include <vector>

#include "./MyStringMacros.h"





class MyStringWide;

class MyConstString;


class MyStringAnsi
{
public:
	typedef enum SearchAlgorithm { BM = 0, KMP = 1, BF = 2, C_LIB = 3 } SearchAlgorithm;


	MyStringAnsi();
	MyStringAnsi(char * str);
	MyStringAnsi(const char * str);
	MyStringAnsi(const MyStringAnsi & other); //copy ctor
	MyStringAnsi(MyStringAnsi && other);	//move ctor

	MyStringAnsi(const MyConstString& other);
	~MyStringAnsi();

	static MyStringAnsi LoadFromFile(const MyStringAnsi & fileName);
	static MyStringAnsi CreateFormated(const char * str, ...);

	MyStringAnsi CreateReplaced(const char * src, const char * dest) const;

	//======================================================================
	void SetLocale(const char * locale);
	void Release();
	bool SaveToFile(const MyStringAnsi & fileName) const;


	const char * GetConstString() const { return this->str; };
	const char * c_str() const { return this->str; };
	int GetLength() const { return this->length; };

	uint32 GetHashCode() const;
	uint32 GetRawHashCode() const;
	char GetCharAt(int index) const;
	char GetLastChar() const;

	void PrintToConsole() const;
	void FillWideString(wchar_t * &str) const;
	void FillString(char * &str) const;
	void FillString(const char * &str) const;
	bool IsEmpty() const;

	void Append(const char * str);
	void AppendFormat(const char * str, ...);	
	void Replace(const MyStringAnsi & src, const MyStringAnsi & dest);
	void Replace(const char * src, const char * dest);
	void Replace(const char * src, const char * dest, int offset);
	void Replace(const char * search, const char * replace, const std::vector<int> & searchStartPos);
	
	
	

	int Find(const MyStringAnsi & str, SearchAlgorithm algo = C_LIB) const;
	int Find(const char * str, SearchAlgorithm algo = C_LIB) const;
	int Find(const char * str, int offset) const;
	std::vector<int> FindAll(const char * str) const;

	std::vector<MyStringAnsi> Split(const char * delimeters, bool keepEmptyValues = false) const;
	std::vector<MyStringAnsi> Split(std::vector<MyStringAnsi> &delimeters) const;

	MyStringAnsi SubString(int start) const;
	MyStringAnsi SubString(int start, int length) const;
	void CopySubstring(int start, char * destination) const;
	void CopySubstring(int start, int length, char * destination) const;
	

	int Count(const MyStringAnsi & str) const;
	int Count(const char * str) const;

	void Trim();
	void Reverse();
	void ToUpper();
	void ToLower();
	void RemoveDiacritics();
	void RemoveMultipleChars(char t);
	int Compare(const MyStringAnsi & str) const;

	int LevenshteinDistance(const MyStringAnsi & str2) const;
	int LevenshteinDistance(const char * str2) const;

	MyStringAnsi & operator = (const MyStringAnsi & str);
	MyStringAnsi & operator = (const char * str);

	MyStringAnsi& operator=(MyStringAnsi&& other);

	void operator += (const MyStringAnsi & str);
	void operator += (const char * str);
	void operator += (const char letter);
	void operator += (const int number);
	void operator += (const long number);
	void operator += (const uint32 number);
	void operator += (const uint64 number);
	void operator += (const float number);
	void operator += (const double number);
	char & operator [](const int index);
	MyStringAnsi operator + (const MyStringAnsi & str) const;
	MyStringAnsi operator + (const char * str) const;
	MyStringAnsi operator + (const int number) const;
	MyStringAnsi operator + (const double number) const;

	operator uint32() const { return GetHashCode(); }

	//operator const MyConstString &() const;
	//operator const const char *() const { return this->GetConstString(); }

	//conversion
	int ToInt() const;
	double ToReal() const;
	bool IsNumber() const;
	bool IsIntNumber() const;
	bool IsFloatNumber() const;
	std::vector<double> GetAllNumbers() const;
	

	//friends
	friend MyStringAnsi operator +(int value, const MyStringAnsi & right);

	
private:
	char * str;
	int length;	
	volatile uint32 hashCode;
	int bufferSize;



	void ResizeBuffer(int length);
	void CreateNew(const char * str);
	int CLib(const char * str, int start = 0) const;
	int BruteForce(const char * str, int start = 0) const;
	int BoyerMoore(const char * str, int * &lookUp, int start = 0) const;
	int KnuthMorisPrat(const char * str, int * &lookUp, int start = 0) const;

	char * strtok_single(char * str, char const * delims) const;
};



//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash<MyStringAnsi>
	{
		std::size_t operator()(const MyStringAnsi & k) const
		{
			return k.GetHashCode();
		};
	};
};

#include "./ComparisonOperators.inl"

#undef _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#undef _CRT_SECURE_NO_DEPRECATE

#endif