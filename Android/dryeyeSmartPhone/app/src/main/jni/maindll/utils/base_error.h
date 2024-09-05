/*
 * base_error.h
 *
 *  Created on: Mar 24, 2012
 *      Author: alex
 */
#ifndef BASE_ERROR_H_
	#define BASE_ERROR_H_
	#include "BoolWarning.h"
	EXPORTNIX BOOL IsBadReadPtr(void* lp, UINT cb);
	_FEXPORT32 int __CheckError(int err, const char *str, const char *pFile, int line);

	_FEXPORT32 void __ASSERT(bool f, const char *pFile, int line);

	#ifdef NDEBUG
        template<typename T>
		inline void ASSERT(T a) { ; }
		inline int StdError(int a) { return a; }
		#define OutputDebugStringA(a) (a)
	#else
			#define ASSERT(a) (__ASSERT(a, __FILE__ , __LINE__))
			#define StdError(a) (__CheckError(a, 0, __FILE__ , __LINE__)) //_StdError(a, __FILE__ , __LINE__))
			#define OutputDebugStringA printf
			#ifdef BoolWarning
            #undef BoolWarning
			#endif
			#define BoolWarning ASSERT


	#endif

	#ifndef _CONSOLE_PROG_
		_FEXPORT32 void PAssertHandler(const wxString& file, int line, const wxString& func, const wxString& cond, const wxString& msg);
		EXPORTNIX void PrintStackToBuffer(TString &b, int N);
	#endif
#endif /* BASE_ERROR_H_ */
