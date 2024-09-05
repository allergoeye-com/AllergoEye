/*
 * Loger.cpp

 *
 *  Created on: Jul 14, 2014
 *      Author: alex
 */
#include "stdafx.h"

#if (1) //ndef NDEBUG

#pragma pack(push)
#pragma pack(0)

#include <stdarg.h>
#include <android/log.h>
#include "Loger.h"
#include <android/sensor.h>
#include <android/looper.h>

#pragma pack(pop)
EXPORTNIX void PrintF(const char *pMessage, ...)
{
    va_list lVarArgs;
    va_start(lVarArgs, pMessage);
    __android_log_vprint(ANDROID_LOG_ERROR, "TRACE", pMessage,
                         lVarArgs);
    __android_log_print(ANDROID_LOG_ERROR, "TRACE", "\n");
    va_end(lVarArgs);
}

#endif
