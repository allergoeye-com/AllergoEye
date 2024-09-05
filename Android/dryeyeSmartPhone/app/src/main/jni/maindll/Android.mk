LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := maindll
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/uuid
LOCAL_C_INCLUDES += $(LOCAL_PATH)/sinclude
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ADC
LOCAL_C_INCLUDES += $(LOCAL_PATH)/AWnd
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Math
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Algorithm
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Java
LOCAL_C_INCLUDES += $(LOCAL_PATH)/zip
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../freetype/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../freetype/include/freetype

LOCAL_SRC_FILES :=utils/android_excep.cpp utils/minstance.cpp utils/MultiThread.cpp galloc.cpp variable.cpp utils/_ffile.cpp utils/afx.cpp utils/base_error.cpp utils/ExplorThread.cpp \
utils/mapmemfile.cpp utils/mdir.cpp utils/mfile.cpp utils/mpprocess.cpp utils/mpthread.cpp utils/msghtread.cpp utils/CSemaphore.cpp \
utils/uxmessage.cpp utils/RWLock.cpp utils/CPing.cpp utils/CCheckArp.cpp \
utils/uxsignal.cpp utils/virtualalloc.cpp uuid/clear.cpp uuid/compare.cpp uuid/copy.cpp uuid/gen_uuid.cpp uuid/isnull.cpp uuid/pack.cpp \
uuid/parse.cpp uuid/unpack.cpp uuid/unparse.cpp uuid/uuid_time.cpp Loger.cpp CNetLink.cpp \
Java/JVariable.cpp Java/JSpec.cpp Java/AClass.cpp Java/AMember.cpp Java/AObject.cpp Java/CreateClassUtil.cpp \
ADC/AColor.cpp ADC/ADC.cpp ADC/APalette.cpp ADC/AFont.cpp ADC/ABmp.cpp ADC/AClip.cpp ADC/EmbGDISurf.cpp \
ADC/GDIPath.cpp ADC/ILine.cpp ADC/MemImage.cpp ADC/WinBmp.cpp ADC/RgbIndex.cpp ADC/JBitmap.cpp \
AWnd/AWnd.cpp AWnd/Dipatcher.cpp AWnd/DImageList.cpp AWnd/DListView.cpp AWnd/DTabCtrl.cpp AWnd/DSliderCtrl.cpp \
Algorithm/Bmp2PolyLine.cpp Algorithm/ClearBorderVoxels.cpp Algorithm/BuilderVoron2D.cpp Algorithm/LaplasePyramide.cpp Algorithm/fmathimag.cpp Algorithm/FImage.cpp \
Algorithm/RuningPolygon.cpp Algorithm/RC51.cpp zip/lzwcomp.cpp zip/lzwdecom.cpp zip/lzwstrmi.cpp zip/lzwstrmo.cpp




ifeq ($(APP_OPTIM),release)
LOCAL_CPPFLAGS += -DNDEBUG -DNODEBUG
#else
#LOCAL_CPPFLAGS := -fno-omit-frame-pointer -rdynamic
endif

ifeq ($(APP_OPTIM),release)
LOCAL_CFLAGS += -DNDEBUG -DNODEBUG
endif

LOCAL_CPPFLAGS +=  -fPIC -fsigned-char -D_CONSOLE_PROG_ -DANDROID_NDK -D_EOS_LINUX_ -fpack-struct=8
LOCAL_CFLAGS +=  -fPIC -fsigned-char -D_CONSOLE_PROG_ -DANDROID_NDK -D_EOS_LINUX_ -fpack-struct=8
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPP_FEATURES += exceptions
LOCAL_LDFLAGS += -ljnigraphics
LOCAL_STATIC_LIBRARIES := freetype

LOCAL_LDLIBS    := -llog -landroid -lz -ldl

include $(BUILD_SHARED_LIBRARY)
