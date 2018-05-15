APP_ABI := x86_64
APP_PLATFORM := android-27
APP_STL := gnustl_static
# Optimization seems to somehow kill spotifyembedded :( (TLV decode error) so enable as much as possible
#APP_CPPFLAGS += -std=c++14 -g -O0 -fbranch-count-reg -fdefer-pop -finline-functions-called-once -fmerge-constants -fomit-frame-pointer -freorder-blocks -ftree-dce -ftree-ter -funit-at-a-time
APP_CPPFLAGS += -std=c++14 -g
APP_BUILD_SCRIPT := /home/dominik/Documents/libspotify/Android.mk
APP_OPTIM := debug