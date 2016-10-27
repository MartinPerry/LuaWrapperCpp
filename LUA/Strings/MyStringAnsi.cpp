#include "./MyStringAnsi.h"

#include "./MyString.h"
#include "./MurmurHash3.h"
#include "./MyStringWide.h"


#include <cstdio>
#include <cstdlib>
#include <climits>
#include <wctype.h>
#include <cctype>
#include <stdarg.h>

#include <cstring>
#include <locale>
#include <cmath>



#ifndef UINT32_MAX
	#define UINT32_MAX       0xffffffff
#endif



//====================================================================
// ctors / dtors
//====================================================================

MyStringAnsi::MyStringAnsi()
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;

	this->CreateNew(NULL);
}

MyStringAnsi::MyStringAnsi(char * str)
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;

	this->CreateNew(str);
}


MyStringAnsi::MyStringAnsi(const char * str)
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;

	this->CreateNew(str);
}


MyStringAnsi::MyStringAnsi(const MyStringAnsi &other)
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;

	this->CreateNew(other.str);
	this->hashCode = other.GetHashCode();
}

MyStringAnsi::MyStringAnsi(MyStringAnsi && other)  /* noexcept needed to enable optimizations in containers */
{
	//http://blog.smartbear.com/c-plus-plus/c11-tutorial-introducing-the-move-constructor-and-the-move-assignment-operator/

	this->str = other.str;
	this->length = other.length;
	this->bufferSize = other.bufferSize;
	this->hashCode = other.hashCode;

	other.str = NULL;
	other.length = 0;
	other.bufferSize = 0;
	other.hashCode = UINT32_MAX;
}

MyStringAnsi::MyStringAnsi(const MyConstString& other)
{
	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;

	this->CreateNew(other.GetConstString());
	this->hashCode = other.GetHashCode();
}

MyStringAnsi::~MyStringAnsi()
{
	this->Release();
}

/*-----------------------------------------------------------
Function:    Release

Release currently hold data
-------------------------------------------------------------*/
void MyStringAnsi::Release()
{
	SAFE_DELETE_ARRAY(this->str);
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = UINT32_MAX;
}

//====================================================================
// Static methods
//====================================================================

MyStringAnsi MyStringAnsi::LoadFromFile(const MyStringAnsi & fileName)
{
	FILE *f = NULL;  //pointer to file we will read in
	my_fopen(&f, fileName.GetConstString(), "rb");
	if (f == NULL)
	{
		printf("Failed to open file: \"%s\"\n", fileName.c_str());
		return "";
	}

	fseek(f, 0L, SEEK_END);
	long size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char * data = new char[size + 1];
	fread(data, sizeof(char), size, f);
	fclose(f);

	data[size] = 0;
	MyStringAnsi tmp = MyStringAnsi(data);
	SAFE_DELETE_ARRAY(data);

	return tmp;

}

MyStringAnsi MyStringAnsi::CreateFormated(const char * str, ...)
{
	if (str == NULL)
	{
		return "";
	}

	va_list vl;
	

	std::vector<char> localBuffer;
	int appendLength = -1;
	while (appendLength < 0)
	{
		va_start(vl, str);
		localBuffer.resize(localBuffer.size() + 256);
		appendLength = my_vsnprintf(&localBuffer[0], localBuffer.size(), localBuffer.size() - 1, str, vl);
		va_end(vl);
	}
			
	
	MyStringAnsi newStr = MyStringAnsi();
	newStr.ResizeBuffer(appendLength + 10);

	va_start(vl, str);
	int written = my_vsnprintf(newStr.str, newStr.bufferSize, newStr.bufferSize - 1, str, vl);
	va_end(vl);

	

	if (written == -1)
	{
		return "";
	}


	newStr.length = static_cast<int>(strlen(newStr.str));
	newStr.str[newStr.length] = '\0';
	newStr.hashCode = UINT32_MAX;	
	return std::move(newStr);
}

//====================== Methods ===============================

//====================================================================
// Helper methods
//====================================================================

bool MyStringAnsi::SaveToFile(const MyStringAnsi & fileName) const
{

	FILE *f = NULL;  //pointer to file we will read in
	my_fopen(&f, fileName.GetConstString(), "wb");
	if (f == NULL)
	{
		return false;
	}

	fwrite(this->GetConstString(), sizeof(char), this->GetLength(), f);
	fclose(f);

	return true;
}

/*-----------------------------------------------------------
Function:    SetLocale
Parametrs:
[in] locale - locale to set

Set string LC_ALL locale
-------------------------------------------------------------*/
void MyStringAnsi::SetLocale(const char * locale)
{
	setlocale(LC_ALL, locale);
}

/*-----------------------------------------------------------
Function:    PrintToConsole

Perform prinf("%s", string)
-------------------------------------------------------------*/
void MyStringAnsi::PrintToConsole() const
{
	printf("%s", this->str);
}

//====================================================================
// Fill
//====================================================================

/*-----------------------------------------------------------
Function:    FillWideString
Parametrs:
[out] str - filled string

Convert current char * string to wchar_t * string
and filled him to "str"
!Important! str must be freed
-------------------------------------------------------------*/
void MyStringAnsi::FillWideString(wchar_t * &str) const
{
	str = new wchar_t[this->length + 1];
	my_mbstowcs(NULL, str, this->length + 1, this->str, this->length);
	str[this->length] = 0;
}

/*-----------------------------------------------------------
Function:    FillString
Parametrs:
[out] str - filled string

Fill current string to "str"
!Important! str must be freed
-------------------------------------------------------------*/
void MyStringAnsi::FillString(char * &str) const
{
	str = new char[this->length + 1];
	my_strncpy(str, this->length + 1, this->str, this->length);
	str[this->length] = 0;
}

/*-----------------------------------------------------------
Function:    FillString
Parametrs:
[out] str - filled const string

Fill current string to "const str"
!Important! str must be freed
-------------------------------------------------------------*/
void MyStringAnsi::FillString(const char * &str) const
{
	char * tmp = new char[this->length + 1];
	my_strncpy(tmp, this->length + 1, this->str, this->length);
	tmp[this->length] = 0;
	str = my_strdup(tmp);
	SAFE_DELETE_ARRAY(tmp);
}

//====================================================================
// Getters
//====================================================================

uint32 MyStringAnsi::GetHashCode() const
{
	if (this->hashCode != UINT32_MAX)
	{
		return this->hashCode;
	}

	uint32 hash = MurmurHash3_x86_32(this->str, this->length);
	
	const volatile uint32 * ptr = &this->hashCode;
	(*const_cast<uint32*>(ptr)) = hash;

	return this->hashCode;
}

uint32 MyStringAnsi::GetRawHashCode() const
{
	return this->hashCode;
}

char MyStringAnsi::GetCharAt(int index) const
{
	return this->str[index];
}

char MyStringAnsi::GetLastChar() const
{
	return this->str[this->length - 1];
}

int MyStringAnsi::ToInt() const
{
	return atoi(this->str);
}

double MyStringAnsi::ToReal() const
{
	return strtod(this->str, NULL);
}

std::vector<double> MyStringAnsi::GetAllNumbers() const
{
	double num = 0;
	double div = 0;
	bool isNegative = false;
	bool isFractPart = false;
	bool containsNumber = false;


	double ePart = 0;
	bool isEPartActive = false;
	bool isENegative = false;
	std::vector<double> numbers;

	for (int i = 0; i < this->GetLength(); i++)
	{

		if (this->str[i] == '-')
		{
			if (isEPartActive)
			{
				isENegative = true;
			}
			else
			{
				isNegative = true;
			}
		}
		else if (this->str[i] == '.')
		{
			isFractPart = true;
		}
		else if ((this->str[i] == 'e') && (containsNumber))
		{
			isEPartActive = true;
		}
		else if (this->str[i] >= '0' && this->str[i] <= '9')
		{
			if (isEPartActive)
			{
				ePart *= 10;
				ePart += (this->str[i] - '0');
			}
			else
			{

				if (isFractPart)
				{
					div++;
				}

				num *= 10;
				num += (this->str[i] - '0');
			}
			containsNumber = true;
		}
		else if (containsNumber)
		{

			if (isFractPart)
			{
				num /= pow(10.0, div);
			}
			if (isNegative)
			{
				num *= -1;
			}

			if (isEPartActive)
			{
				if (isENegative)
				{
					ePart *= -1;
				}

				num *= pow(10.0, ePart);
			}


			numbers.push_back(num);
			num = 0;
			div = 0;
			ePart = 0;
			isNegative = false;
			isFractPart = false;
			containsNumber = false;
			isEPartActive = false;
			isENegative = false;
		}

	}

	if (containsNumber)
	{
		if (isFractPart)
		{
			num /= pow(10.0, div);
		}
		if (isNegative)
		{
			num *= -1;
		}

		if (isEPartActive)
		{
			if (isENegative)
			{
				ePart *= -1;
			}

			num *= pow(10.0, ePart);
		}

		numbers.push_back(num);
	}

	return numbers;
}

//====================================================================
// Testing if string is something
//====================================================================

/*-----------------------------------------------------------
Function:    IsEmpty
Returns:
true if string has 0 size

Check if string is empty
-------------------------------------------------------------*/
bool MyStringAnsi::IsEmpty() const
{
	return (this->length == 0);
}

bool MyStringAnsi::IsNumber() const
{
	return this->IsFloatNumber();
}

bool MyStringAnsi::IsIntNumber() const
{
	if (this->length == 0)
	{
		return false;
	}

	int startPos = 0;
	if (this->str[0] == '-')
	{
		startPos = 1;
	}

	for (int i = startPos; i < this->length; i++)
	{
		if (isdigit(this->str[i]) == false)
		{
			return false;
		}
	}

	return true;
}

bool MyStringAnsi::IsFloatNumber() const
{
	if (this->length == 0)
	{
		return false;
	}

	int startPos = 0;
	if (this->str[0] == '-')
	{
		startPos = 1;
	}

	int decCount = 0;
	for (int i = startPos; i < this->length; i++)
	{
		if ((this->str[i] == '.') && (decCount == 0))
		{
			decCount++;
			continue;
		}

		if ((this->str[i] < '0') || (this->str[i] > '9'))
		{
			return false;
		}
	}

	return true;
}


//====================================================================
// Substrings
//====================================================================

/*-----------------------------------------------------------
Function:    SubString
Parametrs:
[in] start - start position
Returns:
substring from string

Get substring within string
-------------------------------------------------------------*/
MyStringAnsi MyStringAnsi::SubString(int start) const
{
	int length = this->length - start;

	return this->SubString(start, length);

}

/*-----------------------------------------------------------
Function:    SubString
Parametrs:
[in] start - start position
[in] length - length of substring
Returns:
substring from string

Get substring within string
-------------------------------------------------------------*/
MyStringAnsi MyStringAnsi::SubString(int start, int length) const
{
	if (start + length > this->length)
	{
		//to do ... hodit vyjimku
	}
	char * newStr = new char[length + 1];
	my_strncpy(newStr, length + 1, this->str + start, length);
	newStr[length] = 0;

	MyStringAnsi newAnsiStr;

	SAFE_DELETE_ARRAY(newAnsiStr.str);
	newAnsiStr.str = newStr;
	newAnsiStr.length = length;
	newAnsiStr.bufferSize = length + 1;
	//delete[] newStr;
	return newAnsiStr;

}

void MyStringAnsi::CopySubstring(int start, char * destination) const
{
	int length = this->length - start;

	memcpy(destination, this->str + start, length * sizeof(char));
}

void MyStringAnsi::CopySubstring(int start, int length, char * destination) const
{
	if (start + length > this->length)
	{
		//to do ... hodit vyjimku
	}

	memcpy(destination, this->str + start, length * sizeof(char));
}

//====================================================================
// Comparison
//====================================================================

/*-----------------------------------------------------------
Function:    Compare
Parametrs:
[in] str - second string
Returns:
classic strcmp return value

Comare two string using and return value is by strcmp
-------------------------------------------------------------*/
int MyStringAnsi::Compare(const MyStringAnsi & str) const
{
	return strcmp(this->str, str.str);
}


//====================================================================
// Changing existing string
//====================================================================


/*-----------------------------------------------------------
Function:    Trim

Remove empty chars from front and back of string
-------------------------------------------------------------*/
void MyStringAnsi::Trim()
{
	int newLength = this->length;
	char * start = this->str;
	while ((*this->str > 0) && (isspace(*this->str)))
	{
		this->str++;
		newLength--;
	}

	char * end = this->str + newLength - 1;
	while (end > this->str && (((*end) > 0) && isspace(*end)))
	{
		end--;
		newLength--;
	}


	char * trimed = new char[this->bufferSize];

	my_strncpy(trimed, this->bufferSize, this->str, newLength);
	trimed[newLength] = 0;
	SAFE_DELETE_ARRAY(start);

	this->str = trimed;
	this->length = newLength;
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    Reverse

Reverse string
-------------------------------------------------------------*/
void MyStringAnsi::Reverse()
{
	for (int i = 0; i < this->length >> 1; i++)
	{
		SWAP(this->str[i], this->str[this->length - i - 1]);
	}
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    ToLower

Convert String to lowercase
Classis ANSI chars are converted using tolower from <ctype.h>
Chars from <127, 255> are converted manually
-------------------------------------------------------------*/
void MyStringAnsi::ToLower()
{
	my_strlwr(this->str, this->bufferSize);
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    ToUpper

Convert String to uppercase
Classis ANSI chars are converted using toupper from <ctype.h>
Chars from <127, 255> are converted manually
-------------------------------------------------------------*/
void MyStringAnsi::ToUpper()
{
	my_strupr(this->str, this->bufferSize);
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    RemoveDiacritics

Remove diacritical marks
Supported: ¡·»ËœÔ…ÈÃÏÕÌ“Ú”Ûÿ¯äöçù⁄˙Ÿ˘›˝éû
-------------------------------------------------------------*/
void MyStringAnsi::RemoveDiacritics()
{
	int position = 0, i = 0;
	int symbol = -1;
	char specialChars[] = "¡·»ËœÔ…ÈÃÏÕÌ“Ú”Ûÿ¯äöçù⁄˙Ÿ˘›˝éû";
	char replaceToChars[] = "AaCcDdEeEeIiNnOoRrSsTtUuUuYyZz";
	int specialSize = 31;//(int)sizeof(specialChars);

	for (position = 0; position < this->length; position++)
	{
		symbol = this->str[position];
		for (i = 0; i < specialSize; i++)
		{
			if (symbol == specialChars[i])
			{
				this->str[position] = replaceToChars[i];
				break;
			}
		}
	}
	this->hashCode = UINT32_MAX;

}

void MyStringAnsi::RemoveMultipleChars(char t)
{
	std::vector<int> keepIndices;

	keepIndices.push_back(0);
	for (int i = 1; i < this->length; i++)
	{
		if ((this->str[i - 1] == t) && (this->str[i] == t))
		{
			//duplicate		
			if (this->str[keepIndices.back()] != t)
			{
				keepIndices.push_back(i - 1);
			}
		}
		else
		{
			keepIndices.push_back(i);
		}

	}

	char * newStr = new char[keepIndices.size() + 1];
	for (size_t i = 0; i < keepIndices.size(); i++)
	{
		newStr[i] = this->str[keepIndices[i]];
	}

	newStr[keepIndices.size()] = 0;

	SAFE_DELETE_ARRAY(this->str);
	this->str = newStr;
	this->length = keepIndices.size();
	this->bufferSize = keepIndices.size() + 1;
	this->hashCode = UINT32_MAX;

}


/*-----------------------------------------------------------
Function:    Append
Parameters:
[in] str - text to find

Add new string at the end of current
-------------------------------------------------------------*/
void MyStringAnsi::Append(const char * str)
{
	if ((this->str == NULL) || (str == NULL))
	{
		return;
	}
	int newLength = static_cast<int>(strlen(str));

	this->ResizeBuffer(this->length + newLength);

	my_strncpy(this->str + this->length, this->bufferSize - this->length, str, newLength);
	//strncpy(this->str + this->length, str, newLength);

	this->length += newLength;
	this->str[this->length] = '\0';
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    AppendFormat
Parameters:
[in] str - text to find
[in] ... - input elements

Append new formated string
Really SLOW !!!! - because of used vsnprintf
eg ("Formated %d %d", 10, 20) => "Formated 10, 20"
-------------------------------------------------------------*/
void MyStringAnsi::AppendFormat(const char * str, ...)
{

	if ((this->str == NULL) || (str == NULL))
	{
		return;
	}

	va_list vl;
	

	std::vector<char> localBuffer;
	int appendLength = -1;
	while (appendLength < 0)
	{
		va_start(vl, str);
		localBuffer.resize(localBuffer.size() + 256);
		appendLength = my_vsnprintf(&localBuffer[0], localBuffer.size(), localBuffer.size() - 1, str, vl);
		va_end(vl);
	}
	

	this->ResizeBuffer(this->length + appendLength);
	
	va_start(vl, str);
	int written = my_vsnprintf(this->str + this->length, this->bufferSize - this->length, appendLength, str, vl);	
	va_end(vl);

	if (written == -1)
	{
		return;
	}


	this->length = static_cast<int>(strlen(this->str));
	this->str[this->length] = '\0';
	this->hashCode = UINT32_MAX;
}

/*-----------------------------------------------------------
Function:    CreateNew
Parameters:
[in] str - new string

Delete actual string and create new one, if NULL str is passed,
new string will only consist of terminating char '\0'
-------------------------------------------------------------*/
void MyStringAnsi::CreateNew(const char * str)
{



	if (str == NULL)
	{
		SAFE_DELETE_ARRAY(this->str);    //delete actual string... if NULL, than no effect

		this->bufferSize = 1;
		this->length = 0;
		this->str = new char[this->bufferSize];
		this->str[0] = 0;
		return;
	}


	this->length = static_cast<int>(strlen(str));

	if ((this->str != NULL) || (this->bufferSize < this->length + 1))
	{
		SAFE_DELETE_ARRAY(this->str);    //delete actual string... if NULL, than no effect
										 //delete only if buffer size is smaller than strLen			
		
		while (this->bufferSize < this->length + 1)
		{
			this->bufferSize = (this->bufferSize * 3) / 2 + 1;    //recount buffer size
		}

		this->str = new char[this->bufferSize];
	}

	my_strncpy(this->str, this->bufferSize, str, this->length);
	this->str[this->length] = 0;
	this->hashCode = UINT32_MAX;

}

/*-----------------------------------------------------------
Function:    ResizeBuffer
Parameters:
[in] length - new buffer size

Resize internal string buffer
Current string is coppied to the resized one
-------------------------------------------------------------*/
void MyStringAnsi::ResizeBuffer(int length)
{
	if (this->bufferSize > length + 1)
	{
		return;
	}

	while (this->bufferSize < length + 1)
	{
		this->bufferSize = (this->bufferSize * 3) / 2 + 1;
	}

	char * newStr = new char[this->bufferSize];
	my_strncpy(newStr, this->bufferSize, this->str, this->length);
	SAFE_DELETE_ARRAY(this->str);
	this->str = newStr;

}


//====================================================================
// Finding, splitting, replacing
//====================================================================

/*-----------------------------------------------------------
Function:    Find
Parameters:
[in] str - text to find
[in] algo - specify searching algorithm
Returns:
position of occurence needle in haystack

Find str within this string and return first position of
occurence
-------------------------------------------------------------*/
int MyStringAnsi::Find(const MyStringAnsi & str, SearchAlgorithm algo) const
{
	return this->Find(str.str, algo);
}

/*-----------------------------------------------------------
Function:    Find
Parameters:
[in] str - text to find
[in] algo - specify searching algorithm (default: BF)
Returns:
position of occurence needle in haystack (-1 not found)

Find str within this string and return first position of
occurence
-------------------------------------------------------------*/
int MyStringAnsi::Find(const char * str, SearchAlgorithm algo) const
{
	int * last = NULL;
	int pos = -1;
	if (algo == BM)
	{
		pos = this->BoyerMoore(str, last);
	}
	else if (algo == KMP)
	{
		pos = this->KnuthMorisPrat(str, last);
	}
	else if (algo == BF)
	{
		pos = this->BruteForce(str);
	}
	else if (algo == C_LIB)
	{
		pos = this->CLib(str);
	}

	if (last != NULL)
	{
		SAFE_DELETE_ARRAY(last);
	}

	return pos;
}

int MyStringAnsi::Find(const char * str, int offset) const
{
	int count = 0;
	int searchLength = static_cast<int>(strlen(str));
	int * last = NULL;
	int pos = 0;

	while (1)
	{
		pos = this->KnuthMorisPrat(str, last, pos); //better use this, because BM skipping
													//is calculated from haystack, not needle
		if (pos == -1){ break; }        //not found

		if (count == offset)
		{
			break;
		}

		count++;
		pos += searchLength;        //set new search start
	}

	SAFE_DELETE_ARRAY(last);

	return pos;
}

std::vector<int> MyStringAnsi::FindAll(const char * str) const
{
	int searchLength = static_cast<int>(strlen(str));
	int * last = NULL;
	int pos = 0;

	std::vector<int> startPos;
	int foundOffset = 0;

	while (1)
	{
		pos = this->KnuthMorisPrat(str, last, pos); //better use this, because BM skipping
													//is calculated from haystack, not needle
		if (pos == -1){ break; }        //not found
		startPos.push_back(pos);    //store found start positions of serach words
		
		pos += searchLength;        //set new search start

		foundOffset++;
	}

	SAFE_DELETE_ARRAY(last);

	return startPos;
}


char * MyStringAnsi::strtok_single(char * str, char const * delims) const
{
	static char  * src = NULL;

	char  *  p, *ret = 0;

	if (str != NULL)
		src = str;

	if (src == NULL)
		return NULL;


	if ((p = strpbrk(src, delims)) != NULL) {
		*p = 0;
		ret = src;
		src = ++p;
	}

	if ((ret == NULL) && (src != NULL))
	{
		ret = src;
		src = NULL;
	}

	return ret;
}

/*-----------------------------------------------------------
Function:    Split
Parameters:
[in] delimeters - array of char delimeters
Returns:
vector of new splitted strings

Split current string by delimeters
-------------------------------------------------------------*/
std::vector<MyStringAnsi> MyStringAnsi::Split(const char * delimeters, bool keepEmptyValues) const
{

	char * tmp = new char[this->length + 1];
	my_strncpy(tmp, this->length + 1, this->str, this->length);
	tmp[this->length] = 0;

	char* context = NULL;

	std::vector<MyStringAnsi> splited;
	char * ptr = NULL;
	
	if (keepEmptyValues)
	{
		ptr = this->strtok_single(tmp, delimeters);
	}
	else 
	{
		ptr = my_strtok(tmp, delimeters, &context);
	}
	while (ptr != NULL)
	{
		if (*ptr)
		{
			splited.push_back(MyStringAnsi(ptr));
		}
		else 
		{
			splited.push_back(MyStringAnsi(""));
		}
		if (keepEmptyValues)
		{
			ptr = this->strtok_single(NULL, delimeters);
		}
		else 
		{
			ptr = my_strtok(NULL, delimeters, &context);
		}
	}

	SAFE_DELETE_ARRAY(tmp);

	

	return splited;

}

std::vector<MyStringAnsi> MyStringAnsi::Split(std::vector<MyStringAnsi> &delimeters) const
{
	std::vector<MyStringAnsi> splited;

	//TO DO

	return splited;
}

/*-----------------------------------------------------------
Function:    Count
Parameters:
[in] str - text to find
Returns:
number of occurence of str int String

Find number of occurence of "str" int string
-------------------------------------------------------------*/
int MyStringAnsi::Count(const MyStringAnsi & str) const
{
	return this->Count(str.str);
}

/*-----------------------------------------------------------
Function:    Count
Parameters:
[in] str - text to find
Returns:
number of occurence of str int String

Find number of occurence of "str" int string
-------------------------------------------------------------*/
int MyStringAnsi::Count(const char * str) const
{
	std::vector<int> tmp = this->FindAll(str);
	return tmp.size();
}


MyStringAnsi MyStringAnsi::CreateReplaced(const char * src, const char * dest) const
{
	MyStringAnsi tmp = *this;
	tmp.Replace(src, dest);
	return tmp;
}



void MyStringAnsi::Replace(const MyStringAnsi & src, const MyStringAnsi & dest)
{
	this->Replace(src.c_str(), dest.c_str(), -1);
}


void MyStringAnsi::Replace(const char * search, const char * replace)
{
	this->Replace(search, replace, -1);
}

/*-----------------------------------------------------------
Function:    Replace
Parameters:
[in] search - text to find (co)
[in] replace - text to replace (za co)
[in] offset - if multiple occurence of "search", which one to replace (-1 = all)

Replace all "search" with "replace"
Use KMP for finding "search" in text
-------------------------------------------------------------*/
void MyStringAnsi::Replace(const char * search, const char * replace, int offset)
{
	int searchLength = static_cast<int>(strlen(search));
	int * last = NULL;
	int pos = 0;

	std::vector<int> startPos;
	int foundOffset = 0;

	while (1)
	{
		pos = this->KnuthMorisPrat(search, last, pos); //better use this, because BM skipping
		//is calculated from haystack, not needle
		if (pos == -1){ break; }        //not found
		if (offset == -1)
		{
			startPos.push_back(pos);    //store all found start positions of search words
		}
		else if (foundOffset == offset)
		{
			startPos.push_back(pos);    //store found start positions of search words
			break;
		}
		pos += searchLength;        //set new search start

		foundOffset++;
	}

	SAFE_DELETE_ARRAY(last);

	this->Replace(search, replace, startPos);
}

void MyStringAnsi::Replace(const char * search, const char * replace, const std::vector<int> & searchStartPos)
{
	if (searchStartPos.size() == 0)
	{
		//nothing found
		return;
	}

	int searchLength = static_cast<int>(strlen(search));
	int replaceLength = static_cast<int>(strlen(replace));

	int newLength = static_cast<int>(this->length - searchStartPos.size() * searchLength + searchStartPos.size() * replaceLength); //calculate new string length

	//calculate new buffer size for newLength
	while (this->bufferSize < newLength + 1)
	{
		this->bufferSize = (this->bufferSize * 3) / 2 + 1;
	}

	int lastPos = 0;
	int tmpBufferSize = this->bufferSize;
	char * tmpStr = new char[this->bufferSize];

	char * tmpStart = tmpStr;
	char * strStart = this->str;

	std::vector<int>::const_iterator it;
	for (it = searchStartPos.begin(); it != searchStartPos.end(); it++)
	{
		int pos = (*it) - lastPos;
		my_strncpy(tmpStr, tmpBufferSize, this->str, pos); //do tmpStr nakopiruju aktualni od 0 - start nalezeni

		my_strncpy(tmpStr + pos, tmpBufferSize - pos, replace, replaceLength); //za to nakopiruju nahrazeny



		lastPos += pos + searchLength;

		tmpStr += (pos + replaceLength);
		tmpBufferSize -= (pos + replaceLength);

		this->str += pos + searchLength;

	}

	my_strncpy(tmpStr, tmpBufferSize, this->str, strlen(this->str));

	SAFE_DELETE_ARRAY(strStart);
	this->str = tmpStart;

	this->length = newLength;

	this->str[this->length] = '\0';

	
	this->hashCode = UINT32_MAX;
}

//====================================================================
// String differences
//====================================================================

/*-----------------------------------------------------------
Function:    LevenshteinDistance
Parameters:
[in] str2 - second string
Returns:
levenshtein distance

Compute levenshtein distance
-------------------------------------------------------------*/
int MyStringAnsi::LevenshteinDistance(const MyStringAnsi & str2) const
{
	return this->LevenshteinDistance(str2.str);
}

/*-----------------------------------------------------------
Function:    LevenshteinDistance
Parameters:
[in] str2 - second string
Returns:
levenshtein distance

Compute levenshtein distance
-------------------------------------------------------------*/
int MyStringAnsi::LevenshteinDistance(const char * str2) const
{
	int str2Len = strlen(str2);

	int ** table = new int*[this->length + 1];
	for (int i = 0; i <= this->length; i++)
	{
		table[i] = new int[str2Len + 1];
		table[i][0] = i;
	}

	for (int i = 0; i <= str2Len; i++)
	{
		table[0][i] = i;
	}

	for (int i = 1; i <= this->length; i++)
	{
		for (int j = 1; j <= str2Len; j++)
		{
			if (this->str[i - 1] == str2[j - 1])
			{
				table[i][j] = table[i - 1][j - 1];
			}
			else
			{
				int min = table[i - 1][j]; //nad
				//byte smer = 0;

				if (table[i - 1][j - 1] < min) //sikmo
				{
					min = table[i - 1][j - 1];
					//smer = 1;
				}
				if (table[i][j - 1] < min)
				{
					min = table[i][j - 1]; //vlevo
					//smer = 2;
				}

				table[i][j] = min + 1;
			}
		}
	}

	int result = table[this->length][str2Len];

	for (int i = 0; i <= this->length; i++)
	{
		SAFE_DELETE_ARRAY(table[i]);
	}
	SAFE_DELETE_ARRAY(table);

	return result;
}

//====================================================================
// Searching
//====================================================================

/*-----------------------------------------------------------
Function:    BoyerMoore
Parameters:
[in] needle - text to find
[in/out] last - pointer to array of last function
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom Boyer-Moore searching. Last function can be passed in
in "last". If "last" is NULL, last function is calculated
and filled to "last" array
!Important! "last" array must be freed outside this method
-------------------------------------------------------------*/
int MyStringAnsi::BoyerMoore(const char * needle, int * &last, int start) const
{
	int needleLen = strlen(needle);

	if (last == NULL)
	{
		last = new int[static_cast<int>(UCHAR_MAX) + 1];
		memset(last, -1, UCHAR_MAX);
		for (int i = 0; i < this->length; i++)
		{
			last[static_cast<unsigned char>(this->str[i])] = i;
		}
	}
	int index = needleLen - 1;
	int cmpIndex = index;
	index += start;
	while (index < this->length)
	{
		if (this->str[index] == needle[cmpIndex])
		{
			index--;
			cmpIndex--;
			if (cmpIndex < 0)
			{

				return index + 1;
			}
		}
		else
		{
			int offset = last[static_cast<int>(this->str[index])];
			index = index + needleLen - ((cmpIndex < offset + 1) ? cmpIndex : offset + 1);
			cmpIndex = needleLen - 1;
		}
	}


	return -1;

}

/*-----------------------------------------------------------
Function:    KnuthMorisPrat
Parameters:
[in] needle - text to find
[in/out] last - pointer to array of last function
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom KMP searching. Last function can be passed in
in "last". If "last" is NULL, last function is calculated
and filled to "last" array
!Important! "last" array must be freed outside this method
-------------------------------------------------------------*/
int MyStringAnsi::KnuthMorisPrat(const char * needle, int * &last, int start) const
{
	int index = 1;
	int cmpIndex = 0;
	int needleLen = static_cast<int>(strlen(needle));
	int * failFce = last;

	if (failFce == NULL)
	{
		failFce = new int[needleLen];
		last = failFce;
		//buil Fail fce
		failFce[0] = 0;
		index = 1;
		cmpIndex = 0;
		while (index < needleLen)
		{
			if (needle[index] == needle[cmpIndex])
			{
				failFce[index] = failFce[index - 1] + 1;
				cmpIndex++;
			}
			else
			{
				failFce[index] = 0;
				if ((failFce[index - 1] != 0) && (cmpIndex != 0))
				{
					cmpIndex = 0;
					index--;
				}
			}
			index++;
		}

	}
	index = start;
	cmpIndex = 0;
	while (index < this->length)
	{
		if (this->str[index] == needle[cmpIndex])
		{
			index++;
			cmpIndex++;
			if (cmpIndex == needleLen)
			{

				return (index - cmpIndex);
			}
		}
		else
		{
			if (cmpIndex == 0)
			{
				index++;
			}
			else
			{
				char offset = failFce[cmpIndex - 1];
				cmpIndex = offset;
			}
		}
	}


	return -1;
}

/*-----------------------------------------------------------
Function:    BruteForce
Parameters:
[in] needle - text to find
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom BF searching. Suitable for small strings
-------------------------------------------------------------*/
int MyStringAnsi::BruteForce(const char * needle, int start) const
{
	int needleLen = static_cast<int>(strlen(needle));
	int i = start;
	int j = 0;
	int lastPos = -1;

	while (i < this->length)
	{
		j = 0;
		while (j < needleLen)
		{
			if (this->str[i] == needle[j])
			{
				i++;
				j++;
				lastPos = i;
			}
			else
			{
				i++;
				lastPos = -1;
				break;
			}
		}
		if (lastPos != -1)
		{
			return (lastPos - needleLen);
		}
	}

	return -1;
}

/*-----------------------------------------------------------
Function:    CLib
Parameters:
[in] needle - text to find
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom searching using strstr - standard C library
-------------------------------------------------------------*/
int MyStringAnsi::CLib(const char * needle, int start) const
{
	char * found = strstr(this->str + start, needle);
	if (found == NULL)
	{
		return -1;
	}
	return found - this->str;
}

//====================================================================
// Operators
//====================================================================

/*-----------------------------------------------------------
Function:    operator =
Parametrs:
[in] other - new string
Returns:
new instance of MyStringAnsi

ctor overloaded for creating new instance with =
MyStringAnsi strOld
MyStringAnsi strNew = strOld
-------------------------------------------------------------*/
MyStringAnsi & MyStringAnsi::operator= (const MyStringAnsi & other)
{
	if (this == &other) //they are same
	{
		return *this;
	}


	this->CreateNew(other.str);

	return *this;
}

/*-----------------------------------------------------------
Function:    operator =
Parametrs:
	[in] other - new string
Returns:
	refill current string wtih other

move assignment operator
-------------------------------------------------------------*/
MyStringAnsi& MyStringAnsi::operator = (MyStringAnsi&& other)
{
	if (this == &other)
	{
		*this;
	}
	
	// release the current objectís resources
	this->Release();

	// pilfer otherís resource
	this->str = other.str;
	this->bufferSize = other.bufferSize;
	this->length = other.length;
	this->hashCode = other.hashCode;

	// reset other
	other.str = NULL;
	other.length = 0;
	other.bufferSize = 0;
	other.hashCode = UINT32_MAX;
	
	return *this;
}

/*-----------------------------------------------------------
Function:    operator =
Parametrs:
[in] str - new string
Returns:
new instance of MyStringAnsi

ctor overloaded for creating new instance with =
MyStringAnsi str = "NewInstance"
-------------------------------------------------------------*/
MyStringAnsi & MyStringAnsi::operator= (const char * str)
{
	this->CreateNew(str);
	return *this;
}


/*-----------------------------------------------------------
Function:    operator +=
Parametrs:
[in] str - appended string

Append new string at the end of current string
-------------------------------------------------------------*/
void MyStringAnsi::operator+= (const MyStringAnsi & str)
{
	this->Append(str.str);
}

/*-----------------------------------------------------------
Function:    operator +=
Parametrs:
[in] str - appended string

Append new string at the end of current string
-------------------------------------------------------------*/
void MyStringAnsi::operator+= (const char * str)
{
	if (str == NULL)
	{
		return;
	}
	this->Append(str);
}

void MyStringAnsi::operator+= (const char singleChar)
{
	if (this->bufferSize >= this->length + 2)
	{
		this->str[this->length] = singleChar;
		this->length++;
		this->str[this->length] = '\0';
		this->hashCode = UINT32_MAX;
	}
	else
	{
		char buffer[2];
		buffer[0] = singleChar;
		buffer[1] = '\0';
		this->Append(buffer);
	}
	
}


/*-----------------------------------------------------------
Function:    operator +=
Parametrs:
[in] number - number to append

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringAnsi::operator+= (const int number)
{
	if ((number >= 0) && (number <= 255))
	{
		this->Append(SMALL_NUMBERS_TO_STRING[number]);
		return;
	}

	char buffer[50];
	my_sprintf(buffer, 50, "%d", number);
	this->Append(buffer);
	
}


void MyStringAnsi::operator+= (const long number)
{
	if ((number >= 0) && (number <= 255))
	{
		this->Append(SMALL_NUMBERS_TO_STRING[number]);
		return;
	}

	char buffer[50];
	my_sprintf(buffer, 50, "%ld", number);
	this->Append(buffer);

}


/*-----------------------------------------------------------
Function:    operator +=
Parametrs:
[in] number - number to append

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringAnsi::operator+= (const uint32 number)
{
	if ((number >= 0) && (number <= 255))
	{
		this->Append(SMALL_NUMBERS_TO_STRING[number]);
		return;
	}

	char buffer[50];
	my_sprintf(buffer, 50, "%d", number);
	this->Append(buffer);
	
}

void MyStringAnsi::operator+= (const uint64 number)
{

	char buffer[50];
	my_sprintf(buffer, 50, "%lld", number);
	this->Append(buffer);
}

/*-----------------------------------------------------------
Function:    operator +=
Parametrs:
[in] number - floating point number

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringAnsi::operator+= (const float number)
{
	char buffer[50];
	my_sprintf(buffer, 50, "%g", number);
	this->Append(buffer);

	//to do ... aby nebylo omezeno na buffer[50]
}


void MyStringAnsi::operator+= (const double number)
{
	char buffer[50];
	my_sprintf(buffer, 50, "%.10f", number);
	this->Append(buffer);

	//to do ... aby nebylo omezeno na buffer[50]
}

/*-----------------------------------------------------------
Function:    operator[]
Parametrs:
[in] index - position
Returns:
characted at position index

Classic function of [] operator.. can return or set value
at index
-------------------------------------------------------------*/
char & MyStringAnsi::operator [](const int index)
{
	return this->str[index];
}

/*-----------------------------------------------------------
Function:    operator +
Parametrs:
[in] str - new string

Append new string
-------------------------------------------------------------*/
MyStringAnsi  MyStringAnsi::operator + (const MyStringAnsi & str) const
{
	MyStringAnsi newStr = *this;
	newStr.Append(str.str);
	return std::move(newStr);
}

/*-----------------------------------------------------------
Function:    operator +
Parametrs:
[in] str - new string

Append new string
-------------------------------------------------------------*/
MyStringAnsi  MyStringAnsi::operator + (const char * str) const
{
	MyStringAnsi newStr = *this;
	newStr.Append(str);
	return std::move(newStr);
}

/*-----------------------------------------------------------
Function:    operator +
Parametrs:
[in] number - appended number

Append new number
-------------------------------------------------------------*/
MyStringAnsi MyStringAnsi::operator +(const int number) const
{
	MyStringAnsi newStr = *this;
	newStr += number;
	return std::move(newStr);
}

/*-----------------------------------------------------------
Function:    operator +
Parametrs:
[in] number - appended number

Append new number
-------------------------------------------------------------*/
MyStringAnsi MyStringAnsi::operator +(const double number) const
{
	MyStringAnsi newStr = *this;
	newStr += number;
	return std::move(newStr);
}

/*
MyStringAnsi::operator const MyConstString &() const
{
	return reinterpret_cast<const MyConstString &>(*this);
}
*/




//====================== Friend Operators =============================


MyStringAnsi operator +(int value, const MyStringAnsi & right)
{
	MyStringAnsi text = "";
	text += value;
	text += right;

	return std::move(text);
}