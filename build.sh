export NDK_PATH=/root/Android/Sdk/ndk-bundle/
export NDK_PROJECT_PATH=`pwd`
$NDK_PATH/ndk-build NDK_APPLICATION_MK=$NDK_PROJECT_PATH/Application.mk
adb -s emulator-5558 push $NDK_PROJECT_PATH/libs/x86_64/testapp /data/local/test