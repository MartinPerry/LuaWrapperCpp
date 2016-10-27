#ifndef _MY_STRING_H_
#define _MY_STRING_H_



#ifdef _MSC_VER
#define my_strncpy(a, b, c, d) strncpy_s(a, b, c, d)
#define my_strlwr(a, b) _strlwr_s(a, b)
#define my_strupr(a, b) _strupr_s(a, b)
#define my_strtok(a, b, c) strtok_s(a, b, c)
#define my_sprintf(a, b, c, d) sprintf_s(a, b, c, d)
#define my_vsnprintf(a, b, c, e, d) vsnprintf_s(a, b, c, e, d)
#define my_mbstowcs(a, b, c, e, d) mbstowcs_s(a, b, c, e, d)
#define my_swprintf(a, b, c, d) swprintf_s(a, b, c, d)
#define my_strdup(a) _strdup(a)

#define my_wcsdup(a) _wcsdup(a)
#define my_wcstok(a, b, c)  wcstok(a, b)
#define my_wcslwr(a) _wcslwr(a)
#define my_wcsupr(a) _wcsupr(a)

#else

#include <cctype>
#include <cwctype>

#define my_strncpy(a, b, c, d) strncpy(a, c, d)
#define my_strlwr(a, b) do {char * p = a; while ((*p = tolower( *p ))) p++;} while( 0 );
#define my_strupr(a, b) do {char * p = a; while ((*p = toupper( *p ))) p++;} while( 0 );
#define my_strtok(a, b, c) strtok(a, b)
#define my_sprintf(a, b, c, d) sprintf(a, c, d)

#define my_vsnprintf(a, b, c, e, d) vsnprintf(a, c, e, d)
#define my_mbstowcs(a, b, c, e, d) mbstowcs(b, e, d)

#define my_strdup(a) strdup(a)
#define my_swprintf(a, b, c, d) swprintf(a, b, c, d)

#define my_wcsdup(a) wcsdup(a)
#define my_wcstok(a, b, c)  wcstok(a, b, c)

#define my_wcslwr(a) a=my_wcslwr_func(a)
#define my_wcsupr(a) a=my_wcsupr_func(a)



inline wchar_t* my_wcslwr_func(wchar_t* str) { wchar_t* it = str; while (*it != 0) { *it = towlower(*it); ++it; } return str; }

inline wchar_t* my_wcsupr_func(wchar_t* str) { wchar_t* it = str; while (*it != 0) { *it = towupper(*it); ++it; } return str; }

#endif


static const char * SMALL_NUMBERS_TO_STRING[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
	"41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60",
	"61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80",
	"81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100",
	"101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120",
	"121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140",
	"141", "142", "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156", "157", "158", "159", "160",
	"161", "162", "163", "164", "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179", "180",
	"181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192", "193", "194", "195", "196", "197", "198", "199", "200",
	"201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220",
	"221", "222", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236", "237", "238", "239", "240",
	"241", "242", "243", "244", "245", "246", "247", "248", "249", "250", "251", "252", "253", "254", "255"
};

#include "./MyStringMacros.h"
#include "./MyStringAnsi.h"
#include "./MyStringWide.h"
#include "./MyConstString.h"
#include "./MyStringUtils.h"
#include "./ID.h"
#include "./md5.h"
#include "./MurmurHash3.h"



#endif