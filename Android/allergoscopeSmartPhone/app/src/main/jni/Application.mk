APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti
APP_CPPFLAGS := -std=c++11
APP_STL      := c++_shared
APP_CPPFLAGS += -frtti
APP_CPPFLAGS += -fPIE
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -finline-functions
APP_CPPFLAGS += -fpack-struct=8
#debug
APP_CPPFLAGS += -funwind-tables
APP_CPPFLAGS += -rdynamic
APP_CPPFLAGS += -fwrapv
APP_CPPFLAGS += -fstack-protector-all
#release
APP_CPPFLAGS += -O2
APP_OPTIM := release
