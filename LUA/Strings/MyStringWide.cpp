#include "./MyStringWide.h"

#include "./MyString.h"
#include "./MurmurHash3.h"
#include "./MyStringAnsi.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <wctype.h>
#include <ctype.h>
#include <stdarg.h>

#include <string>
#include <locale>

#ifndef UINT32_MAX
	#define UINT32_MAX       0xffffffff
#endif


//====================== Ctors / Dtor ===============================

MyStringWide::MyStringWide()
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;

	this->CreateNew(NULL);
}

MyStringWide::MyStringWide(const wchar_t * str)
{

	
	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;
	
	this->CreateNew(str);	
}

MyStringWide::MyStringWide(const char * str)
{

	
	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;

	this->SetLocale("");
	wchar_t * convert = this->ConvertAnsiToWide(str);

	this->CreateNew(convert);	
	delete[] convert;
}

MyStringWide::MyStringWide(const MyStringWide &other)
{


	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;

	this->CreateNew(other.str);	
}

MyStringWide::MyStringWide(const MyStringAnsi &other)
{

	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;

	
	this->SetLocale("");

	wchar_t * convert = this->ConvertAnsiToWide(other.GetConstString());
	this->CreateNew(convert);	
	delete[] convert;
}

MyStringWide::~MyStringWide()
{

	this->Release();
}

/*-----------------------------------------------------------
Function:	Release

Release currently hold data
-------------------------------------------------------------*/
void MyStringWide::Release()
{		
	if (this->str != NULL)
	{
		delete[] this->str;
	}
	this->str = NULL;
	this->length = 0;
	this->bufferSize = 0;
	this->hashCode = -1;
}





//====================== Methods ===============================

/*-----------------------------------------------------------
Function:	SetLocale
Parametrs:
	[in] locale - locale to set

Set string LC_ALL locale
-------------------------------------------------------------*/
void MyStringWide::SetLocale(const char * locale)
{
	setlocale(LC_ALL, locale);
	//setlocale(LC_ALL, locale)
}

/*-----------------------------------------------------------
Function:	GetConstString
Returns:
	const string

-------------------------------------------------------------*/
const wchar_t * MyStringWide::GetConstString() const
{
	return reinterpret_cast<const wchar_t *>(this->str);
}



/*-----------------------------------------------------------
Function:	FillWideString
Parametrs:
	[out] str - filled string

Fill current string to "str"
!Important! str must be freed
-------------------------------------------------------------*/
void MyStringWide::FillWideString(wchar_t * &str) const
{
	str = new wchar_t[this->length + 1];
	wcsncpy(str, this->str, this->length);
	str[this->length] = 0;
}

/*-----------------------------------------------------------
Function:	FillWideString
Parametrs:
	[out] str - filled const string

Fill current string to "const str"
!Important! str must be freed
-------------------------------------------------------------*/
void MyStringWide::FillWideString(const wchar_t * &str) const
{
	wchar_t * tmp  = new wchar_t[this->length + 1];
	wcsncpy(tmp, this->str, this->length);
	tmp[this->length] = 0;
	str = my_wcsdup(tmp);
	delete[] tmp;
}

/*-----------------------------------------------------------
Function:	GetLength
Returns:
	Length of current string

-------------------------------------------------------------*/
int MyStringWide::GetLength() const
{
	return this->length;
}

uint32 MyStringWide::GetHashCode() const
{
	if (this->hashCode != -1)
	{
		return this->hashCode;
	}

	//uint32 hash = MurmurHash3_x86_32(this->str, this->length);
	uint32 hash = 0;
	/*
	for ( int i = 0; i < this->length; i++ )
    {
		hash = 31 * hash + this->str[i];
    }
	*/

	const volatile uint32 * ptr = &this->hashCode;
	(*const_cast<uint32*>(ptr)) = hash;
	

	return this->hashCode;
}

/*-----------------------------------------------------------
Function:	PrintToConsole

Perform prinf("%s", string)
-------------------------------------------------------------*/
void MyStringWide::PrintToConsole() const
{
	wprintf(L"%s", this->str);
}

/*-----------------------------------------------------------
Function:	IsEmpty
Returns:
	true if string has 0 size

Check if string is empty
-------------------------------------------------------------*/
bool MyStringWide::IsEmpty() const 
{
	return (this->length == 0);
}

/*-----------------------------------------------------------
Function:	Trim

Remove empty chars from front and back of string
-------------------------------------------------------------*/
void MyStringWide::Trim()
{
	int newLength = this->length;
	wchar_t * start = this->str;
	while(iswspace(*this->str)) 
	{
		this->str++;
		newLength--;
	}

	wchar_t * end = this->str + newLength - 1;
	while(end > this->str && iswspace(*end)) 
	{
		end--;
		newLength--;
	}

	

	wchar_t * trimed = new wchar_t[this->bufferSize];


	wcsncpy(trimed, this->str, newLength);
	trimed[newLength] = 0;
	delete[] start;

	this->str = trimed;
	this->length = newLength;
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	Reverse

Reverse string
-------------------------------------------------------------*/
void MyStringWide::Reverse()
{	
	for (int i = 0; i < this->length >> 1; i++)
	{		
		SWAP(this->str[i], this->str[this->length - i - 1]);		
	}
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	ToLower

Convert String to lowercase
Classis ANSI chars are converted using tolower from <ctype.h>
Chars from <127, 255> are converted manually
-------------------------------------------------------------*/
void MyStringWide::ToLower()
{

	my_wcslwr(this->str);
}

/*-----------------------------------------------------------
Function:	ToUpper

Convert String to uppercase
Classis ANSI chars are converted using toupper from <ctype.h>
Chars from <127, 255> are converted manually
-------------------------------------------------------------*/
void MyStringWide::ToUpper()
{
   
	my_wcsupr(this->str);
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	RemoveDiacritics

Remove diacritical marks 
Supported: "ÁáČčĎďÉéĚěÍíŇňÓóŘřŠšŤťÚúŮůÝýŽž"
-------------------------------------------------------------*/
void MyStringWide::RemoveDiacritics()
{
	int position = 0, i = 0;
    wchar_t symbol;
    
	const wchar_t specialChars[] = L"ŮůŽžŘřŠšŤťŇňČčàáảãạăằắẳẵặâầấẩẫậÀÁẢÃẠĂẰẮẲẴẶÂẦẤẨẪẬÒÒÓỎÕỌÔỒỐỔỖỘƠỜỚỞỠỢĚÈÉẺẼẸÊỀẾỂỄỆÙÚỦŨỤƯỪỨỬỮỰÌÍỈĨỊỲÝỶỸỴĎďĐòóỏõọôồốổỗộơờớởỡợěèéẻẽẹêềếểễệùúủũụưừứửữựìíỉĩịíýỳýỷỹỵđď";
	const wchar_t replaceToChars[] = L"UuZzRrSsTtNnCcaaaaaaaaaaaaaaaaaAAAAAAAAAAAAAAAAAOOOOOOOOOOOOOOOOOOEEEEEEEEEEEEUUUUUUUUUUUIIIIIYYYYYDdDoooooooooooooooooeeeeeeeeeeeeuuuuuuuuuuuiiiiiiyyyyyydd";
		
	int specialSize = 156;//(int)sizeof(specialChars);
	
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
	this->hashCode = -1;

}

/*-----------------------------------------------------------
Function:	SubString
Parametrs:
	[in] start - start position
	[in] length - length of substring
Returns:
	substring from string

Get substring within string
-------------------------------------------------------------*/
MyStringWide MyStringWide::SubString(int start, int length) const
{
	if (length > this->length) 
	{
		//to do ... hodit vyjimku
	}
	wchar_t * newStr = new wchar_t[length + 1];
	wcsncpy(newStr, this->str + start, length);
	newStr[length] = 0;

	MyStringWide newWideStr = newStr;
	delete[] newStr;
	return newWideStr;

}

/*-----------------------------------------------------------
Function:	Compare
Parametrs:
	[in] str - second string
Returns:
	classic strcmp return value

Comare two string using and return value is by strcmp
-------------------------------------------------------------*/
int MyStringWide::Compare(const MyStringWide & str) const
{
	return wcscmp(this->str, str.str);
}

/*-----------------------------------------------------------
Function:	Append
Parameters:	
	[in] str - text to find	

Add new string at the end of current
-------------------------------------------------------------*/
void MyStringWide::Append(const wchar_t * str)
{
	if ((this->str == NULL) || (str == NULL))
	{
		return;
	}
	int newLength = wcslen(str);

	this->ResizeBuffer(this->length + newLength);
	
	wcsncpy(this->str + this->length, str, newLength);
	
	
	this->length += newLength;	
	this->str[this->length] = '\0';
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	Append
Parameters:	
	[in] str - text to find	

Add new string at the end of current
-------------------------------------------------------------*/
void MyStringWide::Append(const char * str)
{
	if ((this->str == NULL) || (str == NULL))
	{
		return;
	}
	wchar_t * convert = this->ConvertAnsiToWide(str);
	this->Append(convert);
	delete[] convert;
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	AppendFormat
Parameters:	
	[in] str - text to find	
	[in] ... - input elements

Append new formated string
Really SLOW !!!! - because of used vsnprintf
eg ("Formated %d %d", 10, 20) => "Formated 10, 20"
-------------------------------------------------------------*/
void MyStringWide::AppendFormat(const wchar_t * str, ...)
{
	
	if ((this->str == NULL) || (str == NULL))
	{
		return;
	}

	va_list vl;
	va_start(vl, str);
	 
	int newLength = this->length + 10 * wcslen(str);
	this->ResizeBuffer(this->length + newLength);

	vswprintf(this->str + this->length, newLength, str, vl);

	va_end(vl);

	this->length = wcslen(this->str);	
	this->str[this->length] = '\0';
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	CreateNew
Parameters:	
	[in] str - new string

Delete actual string and create new one, if NULL str is passed,
new string will only consist of terminating char '\0'
-------------------------------------------------------------*/
void MyStringWide::CreateNew(const wchar_t * str)
{	

	if (str == NULL)
	{
		this->bufferSize = 1;
		this->length = 0;		
		this->str = new wchar_t[this->bufferSize];
		this->str[0] = 0;
		return;
	}
	this->length = wcslen(str);

	if ((this->str != NULL) || (this->bufferSize < this->length + 1))
	{
		delete[] this->str;	//delete actual string... if NULL, than no effect
							//delete only if buffer size is smaller than strLen		
		while(this->bufferSize < this->length + 1)
		{
			this->bufferSize = (this->bufferSize * 3) / 2 + 1;	//recount buffer size			
		}
		
		this->str = new wchar_t[this->bufferSize];
	}

	wcsncpy(this->str, str, this->length);
	this->str[this->length] = 0;
	this->hashCode = -1;
		
}

/*-----------------------------------------------------------
Function:	ResizeBuffer
Parameters:	
	[in] length - new buffer size

Resize internal string buffer
Current string is coppied to the resized one
-------------------------------------------------------------*/
void MyStringWide::ResizeBuffer(int length)
{
	if (this->bufferSize > length + 1)
	{
		return;
	}
	
	while(this->bufferSize < length + 1)
	{
		this->bufferSize = (this->bufferSize * 3) / 2 + 1;				
	}

	wchar_t * newStr = new wchar_t[this->bufferSize];
	wcsncpy(newStr, this->str, this->length);
	delete[] this->str;
	this->str = newStr;


}

/*-----------------------------------------------------------
Function:	Find
Parameters:	
	[in] str - text to find
	[in] algo - specify searching algorithm 
Returns:
	position of occurence needle in haystack

Find str within this string and return first position of
occurence
-------------------------------------------------------------*/
int MyStringWide::Find(const MyStringWide & str, SearchAlgorithm algo) const
{
	return this->Find(str.str);
}

/*-----------------------------------------------------------
Function:	Find
Parameters:	
	[in] str - text to find
	[in] algo - specify searching algorithm (default: BF)
Returns:
	position of occurence needle in haystack

Find str within this string and return first position of
occurence
-------------------------------------------------------------*/
int MyStringWide::Find(const wchar_t * str, SearchAlgorithm algo) const
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
		delete[] last;
	}

	return pos;
}

/*-----------------------------------------------------------
Function:	Split
Parameters:	
	[in] delimeters - array of char delimeters
Returns:
	vector of new splitted strings

Split current string by delimeters
-------------------------------------------------------------*/
std::vector<MyStringWide> MyStringWide::Split(const wchar_t * delimeters) const
{
	wchar_t * tmp = new wchar_t[this->length + 1];
	wcsncpy(tmp, this->str, this->length);
	tmp[this->length] = 0;

    wchar_t *state = NULL;
	std::vector<MyStringWide> splited;
	wchar_t * ptr = my_wcstok(tmp, delimeters, &state);
	while(ptr != NULL)
	{
		splited.push_back(MyStringWide(ptr));
		ptr = my_wcstok(NULL, delimeters, &state);
	}
	
	delete[] tmp;

	return splited;
}

std::vector<MyStringWide> MyStringWide::Split(std::vector<MyStringWide> &delimeters) const
{
	std::vector<MyStringWide> splited;

	//TO DO

	return splited;
}

/*-----------------------------------------------------------
Function:	Count
Parameters:	
	[in] str - text to find
Returns:
	number of occurence of str int String

Find number of occurence of "str" int string
-------------------------------------------------------------*/
int MyStringWide::Count(const MyStringWide & str) const
{
	return this->Count(str.str);
}

/*-----------------------------------------------------------
Function:	Count
Parameters:	
	[in] str - text to find
Returns:
	number of occurence of str int String

Find number of occurence of "str" int string
-------------------------------------------------------------*/
int MyStringWide::Count(const wchar_t * str) const
{
	int count = 0;
	int searchLength = wcslen(str);
	int * last = NULL;
	int pos = 0;

	while(1)
	{
		pos = this->KnuthMorisPrat(str, last, pos); //better use this, because BM skipping
													//is calculated from haystack, not needle	
		if (pos == -1){ break; }		//not found
		
		count++;
		pos += searchLength;		//set new search start
	}
	return count;
}

/*-----------------------------------------------------------
Function:	Replace
Parameters:	
	[in] search - text to find (co)
	[in] replace - text to replace (za co)

Replace all "search" with "replace"
Use KMP for finding "search" in text
-------------------------------------------------------------*/
void MyStringWide::Replace(const wchar_t * search, const wchar_t * replace)
{
	int searchLength = wcslen(search);
	int replaceLength = wcslen(replace);
	int * last = NULL;
	int pos = 0;

	std::vector<int> startPos;

	while(1)
	{
		pos = this->KnuthMorisPrat(search, last, pos); //better use this, because BM skipping
													   //is calculated from haystack, not needle	
		if (pos == -1){ break; }		//not found
		
		startPos.push_back(pos);	//store all found start positions of serach words
		pos += searchLength;		//set new search start
	}

	
	int newLength = this->length - startPos.size() * searchLength + startPos.size() * replaceLength; //calculate new string length
	
	//calculate new buffer size for newLength
	while(this->bufferSize < newLength + 1)
	{
		this->bufferSize = (this->bufferSize * 3) / 2 + 1;				
	}

	int lastPos = 0;
	wchar_t * tmpStr = new wchar_t[this->bufferSize];
	
	wchar_t * tmpStart = tmpStr;
	wchar_t * strStart = this->str;

	std::vector<int>::const_iterator it;
	for (it = startPos.begin(); it != startPos.end(); it++)
	{
		pos = (*it) - lastPos;
		wcsncpy(tmpStr, this->str, pos); //do tmpStr nakopiruju aktualni od 0 - start nalezeni

		wcsncpy(tmpStr + pos, replace, replaceLength); //za to nakopiruju nahrazeny


		
		lastPos += pos + searchLength;

		tmpStr += pos + replaceLength;
		this->str += pos + searchLength;
		
	}

	wcsncpy(tmpStr, this->str, wcslen(this->str));

	delete[] strStart;
	this->str = tmpStart;

	this->length = newLength;

	this->str[this->length] = '\0';

	if (last != NULL)
	{
		delete[] last;
	}
	this->hashCode = -1;
}

/*-----------------------------------------------------------
Function:	LevenshteinDistance
Parameters:	
	[in] str2 - second string
Returns:
	levenshtein distance

Compute levenshtein distance
-------------------------------------------------------------*/
int MyStringWide::LevenshteinDistance(const MyStringWide & str2) const
{
	return this->LevenshteinDistance(str2.str);
}

/*-----------------------------------------------------------
Function:	LevenshteinDistance
Parameters:	
	[in] str2 - second string
Returns:
	levenshtein distance

Compute levenshtein distance
-------------------------------------------------------------*/
int MyStringWide::LevenshteinDistance(const wchar_t * str2) const
{
	int str2Len = wcslen(str2);

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
				if(table[i][j - 1] < min)
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
		delete[] table[i];
	}
	delete[] table;

	return result;
}

/*-----------------------------------------------------------
Function:	BoyerMoore
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
int MyStringWide::BoyerMoore(const wchar_t * needle, int * &last, int start) const
{
	int needleLen = wcslen(needle);
	
	if (last == NULL)
	{
		last = new int[static_cast<int>(WCHAR_MAX) + 1];
		memset(last, -1, WCHAR_MAX);	
		for (int i = 0; i < this->length; i++)
		{
			last[static_cast<wchar_t>(this->str[i])] = i;
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
			int offset = last[this->str[index]];
			index = index + needleLen - ((cmpIndex < offset + 1) ? cmpIndex : offset + 1);
			cmpIndex = needleLen - 1;
		}
	}

	
	return -1;

}

/*-----------------------------------------------------------
Function:	KnuthMorisPrat
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
int MyStringWide::KnuthMorisPrat(const wchar_t * needle, int * &last, int start) const
{
	int index = 1;
	int cmpIndex = 0;
	int needleLen = wcslen(needle);
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
				if ((failFce[index - 1] != 0)&&(cmpIndex != 0))
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
Function:	BruteForce
Parameters:	
	[in] needle - text to find	
	[in] start - start position of searching (default: 0)
Returns:
	position of occurence needle in haystack

Perfrom BF searching. Suitable for small strings
-------------------------------------------------------------*/
int MyStringWide::BruteForce(const wchar_t * needle, int start) const
{	
	int needleLen = wcslen(needle);
	int i = start;
	int j = 0;	
	int lastPos = -1;

	while(i < this->length)
	{
		j = 0;
		while(j < needleLen)
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
Function:	CLib
Parameters:	
	[in] needle - text to find	
	[in] start - start position of searching (default: 0)
Returns:
	position of occurence needle in haystack

Perfrom searching using strstr - standard C library 
-------------------------------------------------------------*/
int MyStringWide::CLib(const wchar_t * needle, int start) const
{
	wchar_t * found = wcsstr(this->str + start, needle);
	if (found == NULL)
	{
		return -1;
	}
	return found - this->str;
}

/*-----------------------------------------------------------
Function:	ConvertAnsiToWide
Parameters:	
	[in] src - text to convert	
Returns:
	converted sring to wchar_t

Convert input char * to wchar_t *
!Important! - converted wchar_t * must be freed
-------------------------------------------------------------*/
wchar_t * MyStringWide::ConvertAnsiToWide(const char * src)
{
	int length = strlen(src);
	wchar_t * dest = new wchar_t[length + 1];	
	mbstowcs(dest, src, length);	
	dest[length] = '\0';

	return dest;
}

//====================== Operators ===============================
/*-----------------------------------------------------------
Function:	operator =
Parametrs:
	[in] str - new string
Returns:
	new instance of MyStringWide

ctor overloaded for creating new instance with =
MyStringWide strOld
MyStringWide strNew = strOld
-------------------------------------------------------------*/
MyStringWide & MyStringWide::operator= (const MyStringWide & other)
{
	if (this == &other) //they are same
	{
		return *this;
	}
	
	
	this->CreateNew(other.str);
	
	return *this;
}

/*-----------------------------------------------------------
Function:	operator =
Parametrs:
	[in] str - new string
Returns:
	new instance of MyStringWide

ctor overloaded for creating new instance with =
MyStringWide str = "NewInstance"
-------------------------------------------------------------*/
MyStringWide & MyStringWide::operator= (const wchar_t * str)
{		
	this->CreateNew(str);	
	return *this;
}

/*-----------------------------------------------------------
Function:	operator =
Parametrs:
	[in] str - new string
Returns:
	new instance of MyStringWide

ctor overloaded for creating new instance with =
MyStringWide str = "NewInstance"
-------------------------------------------------------------*/
MyStringWide & MyStringWide::operator= (const char * str)
{	
	wchar_t * convert = this->ConvertAnsiToWide(str);
	this->CreateNew(convert);	
	delete[] convert;
	return *this;
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] str - appended string

Append new string at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const MyStringWide & str)
{	
	this->Append(str.str);
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] str - appended string

Append new string at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const wchar_t * str)
{
	if (str == NULL)
	{
		return;
	}
	this->Append(str);
}

void MyStringWide::operator+= (const wchar_t str)
{
	
	wchar_t * buffer = new wchar_t[2];
	buffer[0] = str;
	buffer[1] = 0;
	this->Append(buffer);
	delete[] buffer;
	
}

void MyStringWide::operator+= (const char str)
{
	char * buffer = new char[2];
	buffer[0] = str;
	buffer[1] = 0;
	this->Append(buffer);
	delete[] buffer;
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] str - appended string

Append new ASCII string at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const char * str)
{		
	this->Append(str);	
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] number - number to append

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const int number)
{
	
	wchar_t * buffer = new wchar_t[50];
	int length = my_swprintf(buffer, 50, L"%d", number);
	this->Append(buffer);
	delete[] buffer;
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] number - number to append

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const unsigned int number)
{
	
	wchar_t * buffer = new wchar_t[50];
	int length = my_swprintf(buffer, 50, L"%d", number);
	this->Append(buffer);
	delete[] buffer;
}

void MyStringWide::operator+= (const unsigned long number)
{

	wchar_t * buffer = new wchar_t[50];
	int length = my_swprintf(buffer, 50, L"%ld", number);
	this->Append(buffer);
	delete[] buffer;
}

/*-----------------------------------------------------------
Function:	operator +=
Parametrs:
	[in] number - floating point number

Append number at the end of current string
-------------------------------------------------------------*/
void MyStringWide::operator+= (const double number)
{
	wchar_t * buffer = new wchar_t[50];
	int length = my_swprintf(buffer, 50, L"%g", number);
	this->Append(buffer);
	delete[] buffer;
	//to do ... aby nebylo omezeno na buffer[50]
}

/*-----------------------------------------------------------
Function:	operator[]
Parametrs:
	[in] index - position
Returns:
	characted at position index

Classic function of [] operator.. can return or set value 
at index
-------------------------------------------------------------*/
wchar_t & MyStringWide::operator [](const int index)
{
	return this->str[index];
}

/*-----------------------------------------------------------
Function:	operator +
Parametrs:
	[in] str - new string

Append new string
-------------------------------------------------------------*/
MyStringWide  MyStringWide::operator + (const MyStringWide & str) const
{
	MyStringWide newStr = *this;
	newStr.Append(str.str);
	return newStr;
}

/*-----------------------------------------------------------
Function:	operator +
Parametrs:
	[in] str - new string

Append new string
-------------------------------------------------------------*/
MyStringWide  MyStringWide::operator + (const wchar_t * str) const
{
	MyStringWide newStr = *this;
	newStr.Append(str);
	return newStr;
}

/*-----------------------------------------------------------
Function:	operator +
Parametrs:
	[in] str - new string

Append new string
-------------------------------------------------------------*/
MyStringWide  MyStringWide::operator + (const char * str) const
{
	MyStringWide newStr = *this;
	newStr.Append(str);
	return newStr;
}

/*-----------------------------------------------------------
Function:	operator +
Parametrs:
	[in] number - appended number

Append new number
-------------------------------------------------------------*/
MyStringWide MyStringWide::operator +(const int number) const
{
	MyStringWide newStr = *this;
	newStr += number;
	return newStr;
}

/*-----------------------------------------------------------
Function:	operator +
Parametrs:
	[in] number - appended number

Append new number
-------------------------------------------------------------*/
MyStringWide MyStringWide::operator +(const double number) const
{
	MyStringWide newStr = *this;
	newStr += number;
	return newStr;
}


//====================== Friend Operators =============================
/*-----------------------------------------------------------
Function:	operator ==
Parametrs:
	[in] str1 - first string
	[in] str2 - second string
Returns:
	true is same

Compares two strings
-------------------------------------------------------------*/
bool operator == (const MyStringWide &str1, const MyStringWide &str2)
{
	if (str1.length != str2.length) return false;
	return (wcscmp(str1.str, str2.str) == 0);
}

/*-----------------------------------------------------------
Function:	operator >
Parametrs:
	[in] str1 - first string
	[in] str2 - second string
Returns:
	true if first is greater

Compares two strings
-------------------------------------------------------------*/
bool operator > (const MyStringWide &str1, const MyStringWide &str2)
{
	return (wcscmp(str1.str, str2.str) > 0);
}

/*-----------------------------------------------------------
Function:	operator <
Parametrs:
	[in] str1 - first string
	[in] str2 - second string
Returns:
	true is second is greater

Compares two strings
-------------------------------------------------------------*/
bool operator < (const MyStringWide &str1, const MyStringWide &str2)
{
	return (wcscmp(str1.str, str2.str) < 0);
}

MyStringWide operator +(int value, const MyStringWide & right)
{
	MyStringWide text = L"";
	text += value;
	text += right;

	return text;
}