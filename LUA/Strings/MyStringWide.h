#ifndef MY_STRING_WIDE_H
#define MY_STRING_WIDE_H

#define _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#define _CRT_SECURE_NO_DEPRECATE


#include <wchar.h>
#include <vector>


#include "./MyStringMacros.h"



class MyStringAnsi;

class MyConstString;


class MyStringWide
{
public:
	typedef enum SearchAlgorithm { BM = 0, KMP = 1, BF = 2, C_LIB = 3 } SearchAlgorithm;


	MyStringWide();
	MyStringWide(const wchar_t * str);
	MyStringWide(const char * str);
	MyStringWide(const MyStringWide& other);
	MyStringWide(const MyStringAnsi& other);
	~MyStringWide();

	
	void SetLocale(const char * locale);
	void Release();

	const wchar_t * GetConstString() const;
	int GetLength() const;
	uint32 GetHashCode() const;

	void PrintToConsole() const;
	void FillWideString(wchar_t * &str) const;
	void FillWideString(const wchar_t * &str) const;
	bool IsEmpty() const;

	void Append(const wchar_t * str);
	void Append(const char * str);
	void AppendFormat(const wchar_t * str, ...);
	void Replace(const wchar_t * src, const wchar_t * dest);

	int Find(const MyStringWide & str, SearchAlgorithm algo = C_LIB) const;
	int Find(const wchar_t * str, SearchAlgorithm algo = C_LIB) const;

	std::vector<MyStringWide> Split(const wchar_t * delimeters) const;
	std::vector<MyStringWide> Split(std::vector<MyStringWide> &delimeters) const;

	MyStringWide SubString(int start, int length) const;

	int Count(const MyStringWide & str) const;
	int Count(const wchar_t * str) const;

	void Trim();
	void Reverse();
	void ToUpper();
	void ToLower();
	void RemoveDiacritics();
	int Compare(const MyStringWide & str) const;

	int LevenshteinDistance(const MyStringWide & str2) const;
	int LevenshteinDistance(const wchar_t * str2) const;

	MyStringWide & operator = (const MyStringWide & str);
	MyStringWide & operator = (const wchar_t * str);
	MyStringWide & operator = (const char * str);


	void operator += (const MyStringWide & str);
	void operator += (const wchar_t * str);
	void operator += (const char * str);
	void operator += (const wchar_t letter);
	void operator += (const char letter);
	void operator += (const int number);
	void operator += (const unsigned int number);
	void operator += (const unsigned long number);
	void operator += (const double number);
	wchar_t & operator [](const int index);
	MyStringWide operator + (const MyStringWide & str) const;
	MyStringWide operator + (const wchar_t * str) const;
	MyStringWide operator + (const char * str) const;
	MyStringWide operator + (const int number) const;
	MyStringWide operator + (const double number) const;



	friend bool operator ==(const MyStringWide &str1, const MyStringWide &str2);
	friend bool operator < (const MyStringWide &str1, const MyStringWide &str2);
	friend bool operator > (const MyStringWide &str1, const MyStringWide &str2);
	friend MyStringWide operator +(int value, const MyStringWide & right);


private:
	wchar_t * str;
	int length;
	int bufferSize;
	volatile uint32 hashCode;


	void ResizeBuffer(int length);
	void CreateNew(const wchar_t * str);
	int CLib(const wchar_t * str, int start = 0) const;
	int BruteForce(const wchar_t * str, int start = 0) const;
	int BoyerMoore(const wchar_t * str, int * &lookUp, int start = 0) const;
	int KnuthMorisPrat(const wchar_t * str, int * &lookUp, int start = 0) const;
	wchar_t * ConvertAnsiToWide(const char * src);
};

//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	

	template <>
	struct hash < MyStringWide >
	{
		std::size_t operator()(const MyStringWide & k) const
		{
			return k.GetHashCode();
		};
	};

};

#undef _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#undef _CRT_SECURE_NO_DEPRECATE

#endif