#ifndef _MY_ID_H_
#define _MY_ID_H_

//ID_DEBUG - in this mode, string is stored together with ID
//-> this mode can not be used together with constexpr !!!
//#define ID_DEBUG



//================================
//forward declaration


class EngineID;

typedef EngineID ID;
typedef EngineID StringID;

//================================
//includes

#ifdef ID_DEBUG
#include <unordered_map>
#include <set>
#include <cassert>
#endif

#include "./MyStringMacros.h"

#include "./MurmurHash3.h"

#include "./MyStringAnsi.h"
#include "./MyConstString.h"


//================================
//class declaration

class EngineID 
{
	public:
#if defined(ID_DEBUG) || defined(CONSTEXPR_DISABLE)
		EngineID(const char * key);
#else
		constexpr EngineID(const char * key);
#endif
		EngineID(const MyStringAnsi & key);
		EngineID(const MyConstString & key);
		EngineID(const EngineID & key);
		EngineID(const uint32 & key);

		inline uint32 GetHashID() const { return this->hashID; };		
		inline uint32 GetHashCode() const { return this->hashID; };
		inline operator uint32() { return this->hashID; }		
		inline operator uint32() const { return this->hashID; }
		
		

			

		
	private:
		#ifdef ID_DEBUG
			MyStringAnsi debugKey;

			static void Check(const MyStringAnsi & key, uint32 hash);
						
		#endif
			const uint32 hashID;
		
		

};

//================================
// add operator() to std namepespace
//this will enabled to use our ID in hash_map

namespace std
{
	template <>
	struct hash < EngineID >
	{
		size_t operator()(const EngineID & k) const
		{
			return k.GetHashID();
		};
	};
};



//================================
//declaration of inline ctor
//this is used mainly for constexpr

#if defined(ID_DEBUG) || defined(CONSTEXPR_DISABLE)
	inline EngineID::EngineID(const char * key)
		: hashID(CALC_HASH(key))
	{
#ifdef ID_DEBUG
		MyStringAnsi debugKey = key;

		EngineID::Check(debugKey, hashID);
#endif
	}
#else
	constexpr inline EngineID::EngineID(const char * key)
		: hashID(CALC_HASH(key))
	{
		
	}
#endif

//other ctors
//those are not constexpr

	inline EngineID::EngineID(const MyStringAnsi & key)
		: hashID(key.GetHashCode())
	{
#ifdef ID_DEBUG
		this->debugKey = key;

		EngineID::Check(debugKey, hashID);
		

#endif	

	}

	inline EngineID::EngineID(const MyConstString & key)
		: hashID(key.GetHashCode())
	{
#ifdef ID_DEBUG
		this->debugKey = key.GetConstString();

		EngineID::Check(debugKey, hashID);
#endif	
	}

	inline EngineID::EngineID(const EngineID & key)
		: hashID(key.GetHashID())
	{
#ifdef ID_DEBUG
		this->debugKey = key.debugKey;

		EngineID::Check(debugKey, hashID);
#endif	
	}

	inline EngineID::EngineID(const uint32 & key)
		: hashID(key)
	{
#ifdef ID_DEBUG
		this->debugKey = "direct_hash_value_";
		this->debugKey += key;

		EngineID::Check(debugKey, hashID);
#endif

	}


#ifdef ID_DEBUG
	inline void EngineID::Check(const MyStringAnsi & key, uint32 hashID)
	{
		static std::unordered_map<uint32, std::set<MyStringAnsi> > collisions;
		
		if (collisions.find(hashID) == collisions.end())
		{
			//hashID not exist yet
			collisions[hashID].insert(key);
			return;
		}

		
		if (collisions[hashID].find(key) == collisions[hashID].end())
		{
			//hashID already exist... only one key can be set for this hashID
			assert(collisions[hashID].size() == 0);
		}

		
		collisions[hashID].insert(key);

	}
#endif
//================================

//define forward declaration
#define GENERATE_OPERATOR_FORWRAD(p1, p2, op) \
	bool operator op(const p1 &str1, const p2 &id2);


//
//same has id can be for different string - possibly test here, 
//but that is not needed for common words or if there is no prior problem
//
#define GENERATE_BODY(p1, p2, op) \
	inline bool operator op(const p1 &str1, const p2 &id2){ \
		return (str1.GetHashCode() op id2.GetHashCode());  \
	}; 

#define GENERATE_BODY2(p1, p2, op) \
	inline bool operator op(const p1 &id1, const p2 &id2){ \
		return (id1 op id2.GetHashCode());  \
	}; 

#define GENERATE_BODY3(p1, p2, op) \
	inline bool operator op(const p1 &id1, const p2 &id2){ \
		return (id1.GetHashCode() op id2);  \
	}; 

//================================

//declare friend operators to compare our ID with different
//classes and structures directly
GENERATE_OPERATOR_FORWRAD(EngineID, EngineID, == );
GENERATE_OPERATOR_FORWRAD(MyConstString, EngineID, == );
GENERATE_OPERATOR_FORWRAD(EngineID, MyConstString, == );
GENERATE_OPERATOR_FORWRAD(MyStringAnsi, EngineID, == );
GENERATE_OPERATOR_FORWRAD(EngineID, MyStringAnsi, == );
GENERATE_OPERATOR_FORWRAD(EngineID, uint32, == );
GENERATE_OPERATOR_FORWRAD(uint32, EngineID, == );

GENERATE_OPERATOR_FORWRAD(EngineID, EngineID, != );
GENERATE_OPERATOR_FORWRAD(MyConstString, EngineID, != );
GENERATE_OPERATOR_FORWRAD(EngineID, MyConstString, != );
GENERATE_OPERATOR_FORWRAD(MyStringAnsi, EngineID, != );
GENERATE_OPERATOR_FORWRAD(EngineID, MyStringAnsi, != );
GENERATE_OPERATOR_FORWRAD(EngineID, uint32, != );
GENERATE_OPERATOR_FORWRAD(uint32, EngineID, != );

GENERATE_OPERATOR_FORWRAD(EngineID, EngineID, >);
GENERATE_OPERATOR_FORWRAD(MyConstString, EngineID, >);
GENERATE_OPERATOR_FORWRAD(EngineID, MyConstString, >);
GENERATE_OPERATOR_FORWRAD(MyStringAnsi, EngineID, >);
GENERATE_OPERATOR_FORWRAD(EngineID, MyStringAnsi, >);
GENERATE_OPERATOR_FORWRAD(EngineID, uint32, >);
GENERATE_OPERATOR_FORWRAD(uint32, EngineID, >);


GENERATE_OPERATOR_FORWRAD(EngineID, EngineID, <);
GENERATE_OPERATOR_FORWRAD(MyConstString, EngineID, <);
GENERATE_OPERATOR_FORWRAD(EngineID, MyConstString, <);
GENERATE_OPERATOR_FORWRAD(MyStringAnsi, EngineID, <);
GENERATE_OPERATOR_FORWRAD(EngineID, MyStringAnsi, <);
GENERATE_OPERATOR_FORWRAD(EngineID, uint32, <);
GENERATE_OPERATOR_FORWRAD(uint32, EngineID, <);

//================================

GENERATE_BODY(MyConstString, EngineID, == );
GENERATE_BODY(EngineID, MyConstString, == );
GENERATE_BODY(MyStringAnsi, EngineID, == );
GENERATE_BODY(EngineID, MyStringAnsi, == );
GENERATE_BODY(EngineID, EngineID, == );
GENERATE_BODY2(uint32, EngineID, == );
GENERATE_BODY3(EngineID, uint32, == );

GENERATE_BODY(MyConstString, EngineID, != );
GENERATE_BODY(EngineID, MyConstString, != );
GENERATE_BODY(MyStringAnsi, EngineID, != );
GENERATE_BODY(EngineID, MyStringAnsi, != );
GENERATE_BODY(EngineID, EngineID, != );
GENERATE_BODY2(uint32, EngineID, != );
GENERATE_BODY3(EngineID, uint32, != );

GENERATE_BODY(MyConstString, EngineID, < );
GENERATE_BODY(EngineID, MyConstString, < );
GENERATE_BODY(MyStringAnsi, EngineID, < );
GENERATE_BODY(EngineID, MyStringAnsi, < );
GENERATE_BODY(EngineID, EngineID, < );
GENERATE_BODY2(uint32, EngineID, < );
GENERATE_BODY3(EngineID, uint32, < );

GENERATE_BODY(MyConstString, EngineID, > );
GENERATE_BODY(EngineID, MyConstString, > );
GENERATE_BODY(MyStringAnsi, EngineID, > );
GENERATE_BODY(EngineID, MyStringAnsi, > );
GENERATE_BODY(EngineID, EngineID, > );
GENERATE_BODY2(uint32, EngineID, > );
GENERATE_BODY3(EngineID, uint32, > );


//================================
//undef defined values

#ifdef GENERATE_OPERATOR_FORWRAD
	#undef GENERATE_OPERATOR_FORWRAD
#endif

#ifdef GENERATE_BODY
	#undef GENERATE_BODY
#endif
#ifdef GENERATE_BODY2
	#undef GENERATE_BODY2
#endif
#ifdef GENERATE_BODY3
	#undef GENERATE_BODY3
#endif

#ifdef ID_DEBUG
	#undef ID_DEBUG
#endif



#endif //_MY_ID_H_