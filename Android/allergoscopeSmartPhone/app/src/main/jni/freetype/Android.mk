
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := freetype
LOCAL_C_INCLUDES := $(LOCAL_PATH)/builds/ansi $(LOCAL_PATH)/include/freetype/config $(LOCAL_PATH)/include $(LOCAL_PATH)/include/freetype

LOCAL_SRC_FILES := \
		src/autofit/autofit.c \
		src/bdf/bdf.c \
		src/cff/cff.c \
		src/base/basepic.c \
		src/base/ftapi.c \
		src/base/ftdbgmem.c \
		src/base/ftdebug.c \
		src/base/ftpic.c \
		src/base/ftbase.c \
		src/base/ftbitmap.c \
		src/base/ftfstype.c \
		src/base/ftgasp.c \
		src/cache/ftcache.c \
		src/base/ftglyph.c \
		src/gzip/ftgzip.c \
		src/base/ftinit.c \
		src/lzw/ftlzw.c \
		src/base/ftstroke.c \
		src/base/ftsystem.c \
		src/smooth/smooth.c \
		src/smooth/ftspic.c \
		src/base/ftbbox.c \
		src/base/ftmm.c \
		src/base/ftpfr.c \
		src/base/ftsynth.c \
		src/base/fttype1.c \
		src/base/ftwinfnt.c \
		src/base/ftlcdfil.c \
		src/base/ftgxval.c \
		src/base/ftotval.c \
		src/base/ftpatent.c \
		src/pcf/pcf.c \
		src/pfr/pfr.c \
		src/pfr/pfrgload.c \
		src/pfr/pfrload.c \
		src/pfr/pfrobjs.c \
		src/pfr/pfrsbit.c \
		src/psaux/psaux.c \
		src/psaux/afmparse.c \
		src/psaux/psconv.c \
		src/psaux/psobjs.c \
		src/psaux/t1decode.c \
		src/pshinter/pshinter.c \
		src/pshinter/pshglob.c \
		src/pshinter/pshpic.c \
		src/pshinter/pshrec.c \
		src/psnames/psmodule.c \
		src/psnames/psnames.c \
		src/psnames/pspic.c \
		src/raster/raster.c \
		src/raster/rastpic.c \
		src/sfnt/sfnt.c \
		src/sfnt/pngshim.c \
		src/sfnt/sfntpic.c \
		src/sfnt/ttbdf.c \
		src/sfnt/ttkern.c \
		src/sfnt/ttload.c \
		src/sfnt/ttmtx.c \
		src/sfnt/ttpost.c \
		src/sfnt/ttsbit.c \
		src/sfnt/sfobjs.c \
		src/sfnt/ttcmap.c \
		src/sfnt/sfdriver.c \
		src/truetype/truetype.c \
		src/type1/t1driver.c \
		src/type1/type1.c \
		src/type1/t1afm.c \
		src/type1/t1gload.c \
		src/type1/t1load.c \
		src/type1/t1objs.c \
		src/type1/t1parse.c\
		src/cid/type1cid.c \
		src/type42/t42objs.c \
		src/type42/t42parse.c \
		src/type42/type42.c \
		src/winfonts/winfnt.c \
		src/cid/cidgload.c \
		src/cid/cidload.c \
		src/cid/cidobjs.c \
		src/cid/cidparse.c \
		src/cid/cidriver.c \
		src/pcf/pcfread.c \
		src/pcf/pcfutil.c \
		src/pcf/pcfdrivr.c \
		src/tools/apinames.c \
		src/bdf/bdfdrivr.c\
		src/bdf/bdflib.c
ifeq ($(APP_OPTIM),release)
LOCAL_CFLAGS += -DNDEBUG -DNODEBUG 
endif
LOCAL_CFLAGS +=  -fPIC -DANDROID_NDK -DFT2_BUILD_LIBRARY=1 -DFT_CONFIG_MODULES_H="<ftmodule.h>" -fpack-struct=8
include $(BUILD_STATIC_LIBRARY)

