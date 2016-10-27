#include "./MyConstString.h"

#include <cstring>

#include "./MyStringAnsi.h"

#include "./MurmurHash3.h"

#include <cmath>


#ifndef UINT32_MAX
	#define UINT32_MAX       0xffffffff
#endif

MyConstString::MyConstString(const MyConstString& other)
	: c_str(other.c_str), 
	  length(other.length),
	  hashCode(other.hashCode)
{	
	
}

MyConstString::MyConstString(const MyStringAnsi& other)
	: c_str(other.GetConstString()), 
	  length(other.GetLength()),
	  hashCode(other.GetRawHashCode())
{
		
}

MyConstString::MyConstString(char * str)
	: c_str(NULL), 
	  length(0),
	  hashCode(UINT32_MAX)
{
	//dummy ctor to prevent 
	//char * s = "Hello";
	//ConstMyString tmp = s;
}



MyConstString::~MyConstString()
{
}




uint32 MyConstString::GetHashCode() const
{
	if (this->hashCode != UINT32_MAX)
	{
		return this->hashCode;
	}

	//calculate in runtime
	uint32 hash = MurmurHash3_x86_32(this->c_str, this->length);


	const volatile uint32 * ptr = &this->hashCode;
	(*const_cast<uint32*>(ptr)) = hash;

	return this->hashCode;
}

char MyConstString::GetCharAt(int index) const
{
	return this->c_str[index];
}


MyConstString & MyConstString::operator= (const char * str)
{
	this->c_str = str;
	this->length = strlen(str);
	this->hashCode = UINT32_MAX;
	return *this;
}

MyConstString & MyConstString::operator= (const MyConstString & str)
{
	this->c_str = str.c_str;
	this->length = str.length;
	this->hashCode = str.hashCode;
	return *this;
}



std::vector<double> MyConstString::GetAllNumbers() const
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

		if (this->c_str[i] == '-')
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
		else if (this->c_str[i] == '.')
		{
			isFractPart = true;
		}
		else if ((this->c_str[i] == 'e') && (containsNumber))
		{
			isEPartActive = true;
		}
		else if (this->c_str[i] >= '0' && this->c_str[i] <= '9')
		{
			if (isEPartActive)
			{
				ePart *= 10;
				ePart += (this->c_str[i] - '0');
			}
			else
			{

				if (isFractPart)
				{
					div++;
				}

				num *= 10;
				num += (this->c_str[i] - '0');
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

	return numbers;
}



