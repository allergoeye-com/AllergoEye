#ifdef _EOS_LINUX_ 
#include "std.h"
#ifdef _CONSOLE_PROG_
#ifdef ANDROID_NDK
#pragma pack(push)
#pragma pack(0)
#include "jni.h"
#include "android/bitmap.h"
#pragma pack(pop)
#include "std.h"

#include "mtree.h"
#include "mstack.h"
#include "hashmap.h"
#include "hashset.h"
#ifndef afx_msg
#define afx_msg virtual
#define DECLARE_MESSAGE_MAP()
#define MB_YES 1
#define MB_NO 2
#define MB_OK 3
#define MB_ICONWARNING 4
#define MB_YESNO 5

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDCLOSE             8


#endif
#include "AColor.h"

#include "AFont.h"
#include "ADC.h"
#include "AWnd.h"
#endif

#include "minstance.h"
#include "mvect.h"
#else
#include "wx_inc.h"
#include "wx_def.h"
#endif
#define _TOUCH_POINT (MInstance::iTouchSlop/5)
#define _TOUCH_FIND ((int)(MInstance::iTouchSlop))
#define F_TOUCH_FIND (MInstance::fTouchSlop)
#define strcpy_s(a, b, c) strcpy(a, c)
#endif








