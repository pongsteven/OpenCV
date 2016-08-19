LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


OpenCV_INSTALL_MODULES := on
OpenCV_CAMERA_MODULES := off

OPENCV_LIB_TYPE :=STATIC

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
include D:\OpenCV-android-sdk\sdk\native\jni\OpenCV.mk
else
include $(OPENCV_MK_PATH)
endif

LOCAL_MODULE := OpenCV

LOCAL_SRC_FILES := com_iview_opencv_util_OpenCVHelper.cpp

LOCAL_LDLIBS +=  -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)