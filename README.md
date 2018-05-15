# libspotify_embedded header

Spotify released a android Java SDK to allow android developers to stream music.
This SDK is a thin java wrapper around a C library called libspotify_embedded_shared.so which
seems to be a rewritten version of the long deprecated libspotify.
The library only contains the playback core and expects the programmer to use the web api to get metadata.
This header contains reverse-engineered prototypes for functions and structures.
There is no support from Spotify for this header and I have no clue how accurate this header is,
therefore use it with caution. It might segfault, crash in various "impossible" ways, burn down your house
and murder everyone you love. Or it might just work as expected.
As far as I can tell it seems to work mostly fine for me, but expect the unexpected.

#### Logindata
Before you continue you need to create a header file called "login_data.h" and enter your spotify account into it.
```c++
#pragma once
#define SP_USER "username"
#define SP_PASSWORD "password"
```
