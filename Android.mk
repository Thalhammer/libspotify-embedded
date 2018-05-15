LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := spotify_embedded
LOCAL_SRC_FILES := libspotify_embedded_shared.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := testapp
LOCAL_SRC_FILES := test.cpp
LOCAL_SHARED_LIBRARIES := spotify_embedded
# Don't strip debug builds
ifeq ($(APP_OPTIM),debug)
    cmd-strip := 
endif
include $(BUILD_EXECUTABLE)
