
#include "util.h"

#include <sys/time.h>
#include <wchar.h>

using namespace std;

//=========================
//   Measuring
//=========================

double cur_time(){
  struct timeval tp[1];
  gettimeofday(tp,0);
  return tp->tv_sec + 1.0e-6 * tp->tv_usec;
}



//============================
//   Multi-Byte String
//===========================


wstring widen(const std::string &src) 
{
	wchar_t *wcs = new wchar_t[src.length() + 1];
	mbstowcs(wcs, src.c_str(), src.length() + 1);
	wstring dest = wcs;
	delete [] wcs;
	return dest;
}

string narrow(const std::wstring &src) 
{
	char *mbs = new char[src.length() * MB_CUR_MAX + 1];
	wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
	string dest = mbs;
	delete [] mbs;
	return dest;
}




