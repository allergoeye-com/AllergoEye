LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := ViewDoc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/sinclude
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/ADC
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/AWnd
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/Math
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/Algorithm
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/Java
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../maindll/zip



LOCAL_SRC_FILES := App.cpp WndImageView.cpp android_view.cpp NetClientThread.cpp RWThread.cpp MDNSStruct.cpp

ifeq ($(APP_OPTIM),release)
LOCAL_CFLAGS += -DNDEBUG -DNODEBUG
endif

LOCAL_CFLAGS +=  -fPIC -fsigned-char -DANDROID_NDK -D_EOS_LINUX_ -D_CONSOLE_PROG_ -fpack-struct=8
LOCAL_CPPFLAGS +=  -fPIC -fsigned-char -DANDROID_NDK -D_EOS_LINUX_ -D_CONSOLE_PROG_ -fpack-struct=8
LOCAL_LDFLAGS += -ljnigraphics
LOCAL_LDLIBS += -lz

LOCAL_SHARED_LIBRARIES := maindll
include $(BUILD_SHARED_LIBRARY)
