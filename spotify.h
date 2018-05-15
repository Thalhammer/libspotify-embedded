#pragma once

/**
 * @file spotify.h
 * @author Dominik Thalhammer
 * @date 19.04.2018
 *  
 * @brief File containing prototypes and structures used by the spotify_embedded c sdk.
 *
 * Spotify released a android Java SDK to allow android developers to stream music.
 * This SDK is a thin java wrapper around a C library called libspotify_embedded_shared.so which
 * seems to be a rewritten version of the long deprecated libspotify.
 * The library only contains the playback core and expects the programmer to use the web api to get metadata.
 * This header contains reverseengineered prototypes for functions and structures.
 * There is no support from Spotify for this header and I have no clue how accurate this header is,
 * therefore use it with caution. It might segfault, crash in various "impossible" ways, burn down your house
 * and murder everyone you love. Or it might just work as expected.
 * As far as I can tell it seems to work mostly fine for me, but expect the unexpected.
 * @see https://developer.spotify.com/technologies/spotify-android-sdk/
 * @see https://thalhammer.it/
 */

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable untested function prototypes
 * 
 * I warned you !
 */
#define SP_UNTESTED_FUNCTIONS 1

/**
 * @brief Current API version
 */
#define SP_APIVERSION 13

/**
 * @brief Errorcodes returned by most API calls.
 * 
 * This enum is most likely not complete and might be partially wrong.
 */
typedef enum {
	/** No error */
	E_OK = 0,
	/** Generic error */
	E_FAILED = 1,
	/** Something in init went wrong */
	E_INITFAILED = 2,
	/** Header, library missmatch */
	E_API_VERSION = 3,
	/** A nullpointer was passed, but a value was expected */
	E_NULL_ARGUMENT = 4,
	/** A parameter exceeded the expected range */
	E_INVALID_ARGUMENT = 5,
	/** SpInit was not yet called but is required for this function */
	E_UNINITIALIZED = 6,
	/** SpInit was already called */
	E_ALREADY_INITIALIZED = 7,
	/** Bad login credentials */
	E_LOGIN_BAD_CREDENTIALS = 8,
	/** Premium is required */
	E_NEEDS_PREMIUM = 9,
	/** The Spotify user is not allowed to log in from this country */
	E_TRAVEL_RESTRICTION = 10,
	/** The application has been banned by Spotify */
	E_APPLICATION_BANNED = 11,
	/** An unspecified login error occurred */
	E_GENERAL_LOGIN_ERROR = 12,
	/** The operation is not supported */
	E_UNSUPPORTED = 13,
	/** The operation is not supported if the device is not the active playback device */
	E_NOT_ACTIVE_DEVICE = 14,
	/** The API has been rate-limited */
	E_API_RATE_LIMITED = 15,

	/** Error range reserved for playback-related errors */
	E_PLAYBACK_ERROR_START = 1000,
	/** An unspecified playback error occurred */
	E_PLAYBACK_ERROR_GENERAL = 1001,
	/** The application has been rate-limited */
	E_PLAYBACK_RATE_LIMITED = 1002,
	/** The Spotify user has reached a capping limit that is in effect in this country and/or for this track */
	E_PLAYBACK_CAPPING_LIMIT_REACHED = 1003,
	/** Cannot change track while ad is playing */
	E_PLAYBACK_AD_IS_PLAYING = 1004,
	/** The track is (temporarily) corrupt in the Spotify catalogue */
	E_PLAYBACK_CORRUPT_TRACK = 1005,
	/** Unable to read all tracks from the playing context */
	E_PLAYBACK_CONTEXT_FAILED = 1006,
	/** Prefetch failed because track was unavailable */
	E_PLAYBACK_PREFETCH_UNAVAILABLE = 1007,
	/** An item is already actively being prefetched */
	E_PLAYBACK_ALREADY_PREFETCHING = 1008,
	/** A permanent error was encountered while reading from a registered file storage callback */
	E_PLAYBACK_STORAGE_READ_ERROR = 1009,
	/** A permanent error was encountered while writing to a registered file storage callback */
	E_PLAYBACK_STORAGE_WRITE_ERROR = 1010,
	/** Prefetched item was not fully downloaded or failed */
	E_PLAYBACK_PREFETCH_DOWNLOAD_FAILED = 1011
} sp_error_t;

/**
 * @brief Type of this device.
 */
typedef enum {
	/** Device type not known */
	DT_UNKNOWN = 0,
	/** A plain old computer */
	DT_COMPUTER = 1,
	/** Some sort of tablet */
	DT_TABLET = 2,
	/** A smartphone */
	DT_SMARTPHONE = 3,
	/** Headless connected speaker */
	DT_SPEAKER = 4,
	/** Smarttv applications */
	DT_TV = 5,
	/** AV Receiver */
	DT_AVR = 6,
	/** Set-Top-Boxes */
	DT_STB = 7,
	/** A audio dongle */
	DT_AUDIODONGLE = 8,
	/** Gameconsoles */
	DT_GAMECONSOLE = 9,
	/** Video casting device */
	DT_CASTVIDEO = 10,
	/** Audio casting device */
	DT_CASTAUDIO = 11,
	/** Cars and similar */
	DT_AUTOMOBILE = 12
} sp_devicetype_t;

/**
 * @brief Network connectivity enum
 * 
 * Used to give spotify a hint about available networks. This helps reduce cost and stutter in mobile situations.
 */
typedef enum {
	/** No network connection available */
	CON_OFFLINE = 0,
	/** Wired, reliable network connection is available */
	CON_WIRED = 1,
	/** Wireless network connections like WLAN */
	CON_WIRELESS = 2,
	/** Mobile data carrier (UMTS,LTE,...) */
	CON_MOBILE = 3
} sp_connectivity_t;

/**
 * @brief Connection state enum
 * 
 * Used in state change callback, might be incomplete
 */
typedef enum {
	/** We just logged in */
	CS_LOGGEDIN = 0,
	/** We logged out */
	CS_LOGGEDOUT = 1,
	/** There is temporary connection error */
	CS_TEMPORARYERROR = 2,
	/** We disconnected from the access point */
	CS_DISCONNECTED = 3,
	/** We try reconnecting */
	CS_RECONNECT = 4,
	/** Initial connect attempt */
	CS_CONNECTING = 5
} sp_con_state_t;

/**
 * @brief Stream quality selection
 */
typedef enum {
	/** Low quality stream on slow networks */
	BR_LOW = 0,
	/** Normal quality stream */
	BR_NORMAL = 1,
	/** High quality stream */
	BR_HIGH = 2
} sp_bitrate_t;

/**
 * @brief Playback notifications
 * 
 * Definitly incomplete
 */
typedef enum {
	/** Playback has just started/resumed */
	PN_PLAY = 0,
	/** Playback is now paused */
	PN_PAUSE = 1,
	/** The current track has changed */
	PN_TRACKCHANGED = 2,
	/** Next track was started (skip forward) */
	PN_NEXT = 3,
	/** Last track was started (prev) */
	PN_PREV = 4,
	/** Shuffle was turned on */
	PN_SHUFFLEON = 5,
	/** Shuffle was turned off */
	PN_SHUFFLEOFF = 6,
	/** Repeat was turned on */
	PN_REPEATON = 7,
	/** Repeat was turned off */
	PN_REPEATOFF = 8,
	/** Playback became active */
	PN_BECAMEACTIVE = 9,
	/** Playback became inactive */
	PN_BECAMEINACTIVE = 10,
	/** Playback permission lost */
	PN_LOSTPERMISSION = 11,
	/** Flush all audio buffers */
	PN_AUDIOFLUSH = 12,
	/** Audio delivery is complete */
	PN_AUDIODELIVERYDONE = 13,
	/** Some context changed ? */
	PN_CONTEXTCHANGED = 14,
	/** Track is completed */
	PN_TRACKDELIVERED = 15,
	/** Track metadata changed */
	PN_METADATACHANGED = 16,
} sp_playbacknotify_t;

/**
 * @brief Structure passed to ::SpZeroConfGetVars
 */
typedef struct {
	/** Some sort of random token (challenge/response ?) */
	char token[150];
	/** SHA1 Hash of unique device id supplied to SpInit */
	char uniqueid_hash[65];
	/** Currently active username */
	char username[65];
	/** Device display name */
	char displayname[65];
	/** Account type (should always be PREMIUM) */
	char accounttype[16];
	/** Devicetype */
	char devicetype[16];
	/** Library version */
	char version[31];
	/** Always 1 */
	uint32_t unknown;
	/** Always "NONE" */
	char unknown2[16];
} sp_zeroconfvars_t;

/**
 * @brief Audio sample format
 */
typedef struct {
	/** @brief Number of Channels */
	int nchannels;
	/** @brief Sampling rate */
	int samplerate;
} sp_sampleformat_t;

/**
 * @brief Callback functions passed to ::SpRegisterDebugCallbacks
 */
typedef struct {
	/**
	 * @brief Print a line of debug output
	 * @param line Char buffer holding the line to print
	 * @param data Pointer passed to ::SpRegisterDebugCallbacks
	 */
	void (*print)(const char* line, void* data);
} sp_debug_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterConnectionCallbacks
 */
typedef struct {
	/**
	 * @brief Called if connection state changed
	 * @param state New connection state
	 * @param data Pointer passed to ::SpRegisterConnectionCallbacks
	 */
	void (*onNotify)(sp_con_state_t state, void* data);
	/**
	 * @brief Called after successfull login
	 * @param blob A blob which can be used for later logins using ::SpConnectionLoginBlob
	 * @param username Username of the current user
	 * @param data Pointer passed to ::SpRegisterConnectionCallbacks
	 */
	void (*onNotifyLoggedIn)(const char* blob, const char* username, void* data);
	/**
	 * @brief Called if we receive a message from the accesspoint server
	 * @param msg The received message
	 * @param data Pointer passed to ::SpRegisterConnectionCallbacks
	 */
	void (*onMessage)(const char* msg, void* data);
} sp_connection_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterContentCallbacks
 */
typedef struct {
	void (*fn1)(const char* uri, long arg1, void* data);
	void (*fn2)(const char* arg1, const char* uri, long arg3, void* data);
	void (*fn3)(long arg1, long arg2, long arg3, long arg4);
} sp_content_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterPlaybackCallbacks
 */
typedef struct {
	/**
	 * @brief Called on playback related notifications
	 * @param notify The event
	 * @param data Pointer passed to ::SpRegisterPlaybackCallbacks
	 */
	int (*onNotify)(sp_playbacknotify_t notify, void* data);
	/**
	 * @brief Called if audio data is available
	 * @param frames Pointer to frame data
	 * @param nframes Number of frames in frames
	 * @param format Pointer to structure describing the sampleformat
	 * @param arg4 No idea yet
	 * @param data Pointer passed to ::SpRegisterPlaybackCallbacks
	 * @return Number of frames consumed
	 */
	unsigned long (*onAudioData)(const short* frames, unsigned long nframes, const sp_sampleformat_t* format, unsigned int arg4, void* data);
	/**
	 * @brief Called on playback seek
	 * @param position New position in milliseconds
	 * @param data Pointer passed to ::SpRegisterPlaybackCallbacks
	 */
	void (*onSeek)(uint64_t position, void* data);
	/**
	 * @brief Called on volume changes
	 * @param volume New volume
	 * @param data Pointer passed to ::SpRegisterPlaybackCallbacks
	 */
	void (*onApplyVolume)(unsigned short volume, void* data);
	/**
	 * @brief Called if track is not available
	 * @param uri URI of track
	 * @param data Pointer passed to ::SpRegisterPlaybackCallbacks
	 */
	void (*onUnavailableTrack)(const char* uri, void* data);
	void (*fn6)(long arg1, long arg2, long arg3, long arg4);
} sp_playback_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterStorageCallbacks
 * 
 * Seems to be some sort of cache storage HAL, needs investigation.
 */
typedef struct {
	/**
	 * @brief Create cache file and allocate enough space.
	 * @param key Cache file name
	 * @param size Final size of the cache file
	 * @param data Pointer passed to ::SpRegisterStorageCallbacks
	 */
	long (*alloc)(const char* key, unsigned int size, void* data);
	/**
	 * @brief Write part of cache file
	 * @param key Cache file name
	 * @param offset Offset in cache file to store data at
	 * @param buf Data to write
	 * @param size Length of buffer to write
	 * @param data Pointer passed to ::SpRegisterStorageCallbacks
	 * @return Number of bytes written
	 */
	long (*write)(const char* key, unsigned int offset, const void* buf, unsigned int size, void* data);
	/**
	 * @brief Read part of cache file
	 * @param key Cache file name
	 * @param offset Offset in cache file to read data from
	 * @param buf Buffer to read into
	 * @param size Length of data to read
	 * @param data Pointer passed to ::SpRegisterStorageCallbacks
	 * @return Number of bytes read
	 */
	long (*read)(const char* key, unsigned int offset, void* buf, unsigned int size, void* data);
	/**
	 * @brief Called after all read/write operations are finished
	 * @param key Cache file name
	 * @param data Pointer passed to ::SpRegisterStorageCallbacks
	 */
	void (*close)(const char* key, void* data);
	void (*fn5)(long arg1, long arg2, long arg3, long arg4);
} sp_storage_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterPrefetchCallbacks
 * 
 * Needs investigation.
 */
typedef struct {
	/**
	 * @brief Called when prefetch is done
	 * @param uri Spotifyuri of prefetched track
	 * @param arg2 ?
	 * @param arg3 ?
	 * @param data Pointer passed to ::SpRegisterPrefetchCallbacks
	 */
	void (*fn)(const char* uri, long arg2, long arg3, void* data);
} sp_prefetch_callbacks_t;


/**
 * @brief Callback functions passed to ::SpRegisterDnsHALCallbacks
 * 
 * Needs investigation.
 */
typedef struct {
	/**
	 * @brief Called when a host name needs to be resolved
	 * @param hostname Hostname/Domain to lookup
	 * @param arg2 ?
	 * @param data Pointer passed to ::SpRegisterDnsHALCallbacks
	 */
	int (*lookup)(const char* hostname, struct sockaddr* arg2, void* data);
} sp_dnshal_callbacks_t;

/**
 * @brief Callback functions passed to ::SpRegisterSocketHALCallbacks
 * 
 * Needs investigation.
 */
typedef struct {
	// Create socket
	int (*fn1)(void* a, void* b, void* c, void* data);
	// Setsockopt
	int (*fn2)(void* a, void* b, void* c, void* data);
	// Close
	int (*fn3)(void* a, void* b, void* c, void* d);
	// Bind
	int (*fn4)(void* a, void* b, void* c, void* d);
	// Listen
	int (*fn5)(void* a, void* b, void* c, void* d);
	// Connect
	int (*fn6)(void* a, void* b, void* data);
	// Accept
	int (*fn7)(void* a, void* b, void* c, void* d);
	// Recv
	int (*fn8)(void* a, void* b, void* c, void* d);
	// Send
	int (*fn9)(void* a, void* b, void* c, void* d);
	// Recvfrom
	int (*fn10)(void* a, void* b, void* c, void* d);
	// Sendto
	int (*fn11)(void* a, void* b, void* c, void* d);
	int (*fn12)(void* a, void* b, void* c, void* d);
	int (*fn13)(void* a, void* b, void* c, void* d);
	int (*fn14)(void* a, void* b, void* c, void* d);
	int (*fn15)(void* a, void* b, void* c, void* d);
	int (*fn16)(void* a, void* b, void* c, void* d);
	// Select ?
	int (*fn17)(int a, void* data);
} sp_sockethal_callbacks_t;

/**
 * @brief Main configuration passed to ::SpInit
 */
typedef struct {
	/** @brief API version, set to SP_API_VERSION */
	uint64_t version;			
	/** @brief Maybe some sort of working memory, sdk allocates 16777216 null bytes here */
	void* wmem;
	/** @brief Size of the memory region allocated at wmem (0x1000000 in sdk, crashes if less than 0x80000) */
	uint64_t wmem_size;
	/** @brief Binary app key issued by spotify */
	const uint8_t* app_key;
	/** @brief Size of app key pointed to by app_key */
	uint64_t app_key_len;
	/** @brief Unique device id, sdk uses android id */
	const char* uniqueid;
	/** @brief Displayname of this device */
	const char* displayname;
	/** @brief Device brand, size >= 1 && <= 32 */
	const char* brand;
	/** @brief No clue, always NULL in sdk */
	void* ptr1;
	/** @brief Device model, size >= 1 && <= 32 */
	const char* model;
	/** @brief No clue, always NULL in sdk */
	void* ptr2;
	/** @brief Clientid */
	const char* clientid;
	/** @brief OS Version (e.g. "7.1.1_x86_64") */
	const char* osversion;
	/** @brief Type of this device */
	sp_devicetype_t devicetype;
	/** @brief Callback called on async errors like invalid login */
	void (*on_error)(sp_error_t error, void* data);
	/** @brief Context passed to on_error */
	void* on_error_context;
	/** @brief No clue, always NULL in sdk */
	void* ptr5;
	/** @brief Only checked if ptr5 is set, always NULL in sdk */
	void* ptr6;
} sp_init_config_t;

/**
 * @brief Structure passed to ::SpGetMetadata
 */
typedef struct {
	/** Name of the playlist (if any) */
	char playlist_title[0x100];
	/** URI of the playlist */
	char playlist_uri[0x80];
	/** Title of the track */
	char track_title[0x100];
	/** URI of the track */
	char track_uri[0x80];
	/** Name of the artist */
	char artist_name[0x100];
	/** URI of the artist */
	char artist_uri[0x80];
	/** Name of the album */
	char album_name[0x100];
	/** URI of the album */
	char album_uri[0x80];
	/** URI of cover art */
	char image_uri[0x80];
	/** Duration of this track in milliseconds */
	uint32_t duration;
	/** Maybe index within playlist */
	uint32_t playlist_idx;
	/** Seems to always be identical with playlist_idx */
	uint32_t arg3;
	/** Trackbitrate in kBit/s */
	uint32_t bitrate;
} sp_metadata_t;

/**
 * @brief Set a debug callback to log all api calls and internal messages.
 * @param cbs Callbackstructure
 * @param data Userpointer passed to callback
 */
extern sp_error_t SpRegisterDebugCallbacks(const sp_debug_callbacks_t* cbs, void* data);

/**
 * @brief Register callbacks called on connection changes.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterConnectionCallbacks(const sp_connection_callbacks_t* cbs, void* data);

/**
 * @brief Register callbacks called on playback changes.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterPlaybackCallbacks(const sp_playback_callbacks_t* cbs, void* data);

/**
 * @brief Get library version string
 */
extern const char* SpGetLibraryVersion(void);

/**
 * @brief Get Brand name as passed to SpInit.
 */
extern const char* SpGetBrandName(void);

/**
 * @brief Get model name as passed to SpInit.
 */
extern const char* SpGetModelName(void);

/**
 * @brief Get current server time.
 * @return Servertime as unix timestamp in milliseconds
 */
extern uint64_t SpGetServerTime(void);

/**
 * @brief Get canonical user name of currently logged in user.
 * Requires a successfull login.
 */
extern const char* SpGetCanonicalUsername(void);

/**
 * @brief Get user product type
 * @param buffer Buffer to store type in
 * @param buflen Length of buffer
 */
extern sp_error_t SpGetProductType(char* buffer, int buflen);

/**
 * @brief Get zeroconf variables.
 */
extern sp_error_t SpZeroConfGetVars(sp_zeroconfvars_t*);

/**
 * @brief Run spotify event loop.
 * Needs to be called in a regular fashion.
 */
extern sp_error_t SpPumpEvents(void);

/**
 * @brief Set displayname after SpInit.
 * @param dname The new display name
 */
extern sp_error_t SpSetDisplayName(const char* dname);

/**
 * @brief Set a hint about device connectivity.
 * @param con The current connectivity mode.
 */
extern sp_error_t SpConnectionSetConnectivity(sp_connectivity_t con);

/**
 * @brief Get connectivity mode, as set by SpConnectionSetConnectivity.
 * Default is CON_WIRED.
 */
extern sp_connectivity_t SpConnectionGetConnectivity(void);

/**
 * @brief Login using username and password.
 * @param user Spotifyusername (Truncated if longer than 64 chars)
 * @param pass Spotifypassword
 */
extern sp_error_t SpConnectionLoginPassword(const char* user, const void* pass);

/**
 * @brief Login using blob issued after previous login.
 * @param user Spotifyusername
 * @param blob Blob as issued by previous login callback.
 */
extern sp_error_t SpConnectionLoginBlob(const char* user, const void* blob);

/**
 * @brief Login using OAuth token issued by webapi. Needs "streaming" scope.
 * @param token Token issued by web api
 */
extern sp_error_t SpConnectionLoginOauthToken(const char* token);

/**
 * @brief Check if user is logged in.
 * @return returns zero if no user is logged in, non zero if a user is logged in
 */
extern unsigned int SpConnectionIsLoggedIn(void);

/**
 * @brief Logout current user.
 */
extern sp_error_t SpConnectionLogout(void);

/**
 * @brief Get valid range for Metadata.
 * @param max Pointer to store maximum value in.
 * @param min Pointer to store minimum value in.
 */
extern sp_error_t SpGetMetadataValidRange(int* max, int* min);

/**
 * @brief Set device group
 * @param group New device group
 */
extern sp_error_t SpSetDeviceIsGroup(int group);

/**
 * @brief Get current playback volume
 * @returns Playbackvolume
 */
extern unsigned int SpPlaybackGetVolume(void);

/**
 * @brief Get current playback position
 * @return Playbackposition in milliseconds
 */
extern unsigned int SpPlaybackGetPosition(void);

/**
 * @brief Set playback bitrate
 * @param rate New playback bitrate
 */
extern sp_error_t SpPlaybackSetBitrate(sp_bitrate_t rate);

/**
 * @brief Update playback volume
 * @param vol New volume
 */
extern sp_error_t SpPlaybackUpdateVolume(unsigned int vol);

/**
 * @brief Start playing a uri
 * @param uri The uri to play
 * @param index playlist position
 * @param posInMs Position in milliseconds to start at
 */
extern sp_error_t SpPlayUri(const char* uri, int index, int posInMs);

/**
 * @brief Initialise library
 * @param config Pointer to startup configuration
 */
extern sp_error_t SpInit(const sp_init_config_t* config);

/**
 * @brief Shutdown client SDK
 */
extern sp_error_t SpFree(void);

/**
 * @brief Get Metadata about current playback
 * @param m Structure to store metadata in
 * @param idx Index of track (-1 previous, 0 current, 1 next)
 */
extern sp_error_t SpGetMetadata(sp_metadata_t* m, int idx);

/**
 * @brief Get URL for a spotify image uri
 * @param uri Spotify uri (spotify:image:....)
 * @param buf Buffer to store the url in (currently it requires at least 64 byte, but this might change in the future).
 * @param buf_size Size of the buffer
 */
extern sp_error_t SpGetMetadataImageURL(const char* uri, char* buf, unsigned long long int buf_size);

/**
 * @brief Register callbacks called on content changes.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterContentCallbacks(const sp_content_callbacks_t* cbs, void* data);

#if SP_UNTESTED_FUNCTIONS

/**
 * @brief Set Maximum volume value
 * @param max Number of volume steps
 */
extern sp_error_t SpSetVolumeSteps(unsigned int max);

/**
 * @brief Get Repeat Mode
 * @return zero=no repeat, non zero=repeat
 */
extern unsigned int SpPlaybackGetRepeatMode(void);

/**
 * @brief Check if this is the active playback device
 * @return zero=inactive, non zero= active
 */
extern unsigned int SpPlaybackIsActiveDevice(void);

/**
 * @brief Check if an ad is currently playing
 * @return zero = normal track, non zero = ad
 */
extern unsigned int SpPlaybackIsAdPlaying(void);

/**
 * @brief Check if a track is currently playing
 * @return zero = not playing, non zero = playing
 */
extern unsigned int SpPlaybackIsPlaying(void);

/**
 * @brief Check if repeat is enabled
 * @return zero=disabled, non zero= enabled
 */
extern unsigned int SpPlaybackIsRepeated(void);

/**
 * @brief Check if shuffle is enabled
 * @return zero=disabled, non zero= enabled
 */
extern unsigned int SpPlaybackIsShuffled(void);

/**
 * @brief Pause playback.
 */
extern sp_error_t SpPlaybackPause(void);

/**
 * @brief Continue playback.
 */
extern sp_error_t SpPlaybackPlay(void);

/**
 * @brief Seek to position.
 * @param posInMs New position in track
 */
extern sp_error_t SpPlaybackSeek(unsigned int posInMs);

/**
 * @brief Skip to next track in queue.
 */
extern sp_error_t SpPlaybackSkipToNext(void);

/**
 * @brief Return to previous track in queue.
 */
extern sp_error_t SpPlaybackSkipToPrev(void);

/**
 * @brief Add a uri to queue that will get played after the current track.
 * @param uri Spotify track uri
 */
extern sp_error_t SpQueueUri(const char* uri);

/**
 * @brief Register callbacks called on prefetch events.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterPrefetchCallbacks(const sp_prefetch_callbacks_t* cbs, void* data);

/**
 * @brief Register callbacks called if the API wants to store cache data.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterStorageCallbacks(const sp_storage_callbacks_t* cbs, void* data);

/**
 * @brief Prefetch a Item into cache to allow for fast playback start.
 * @param uri The uri to add
 * @param arg Unknown, maybe playlist index
 */
extern sp_error_t SpPrefetchItem(const char* uri, unsigned int arg);

/**
 * @brief Stop prefetching
 */
extern sp_error_t SpStopPrefetchingItem(void);

/**
 * @brief Pause ZeroConf announce
 */
extern sp_error_t SpZeroConfAnnouncePause(void);

/**
 * @brief Resume ZeroConf announce
 */
extern sp_error_t SpZeroConfAnnounceResume(void);

/**
 * @brief Change if shuffle is enabled
 * @param enable 1=enabled, 0=disabled
 */
extern sp_error_t SpPlaybackEnableShuffle(int enable);

/**
 * @brief Change if repeat is enabled
 * @param enable 1=enabled, 0=disabled
 */
extern sp_error_t SpPlaybackEnableRepeat(unsigned int enable);

/**
 * @brief Register callbacks called when dns queries need to be made.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterDnsHALCallbacks(const sp_dnshal_callbacks_t* cbs, void* data);

/**
 * @brief Set an alarm clock.
 * @param a ?
 * @param b ?
 * @param c ?
 * @param d ?
 * @return E_UNSUPPORTED
 */
extern sp_error_t SpSetAlarmClock(int a, void* b, unsigned long c, unsigned int d);

/**
 * @brief Cancel an alarm clock.
 * @param a ?
 * @return E_UNSUPPORTED
 */
extern sp_error_t SpCancelAlarmClock(int a);

/**
 * @brief Does not seem to have any visible influence.
 * @param a ?
 */
extern sp_error_t SpSetBackendEnv(int a);

/**
 * @brief Register callbacks called for network communication.
 * @param cbs Callbackstructure
 * @param data Parameter passed to callbacks
 */
extern sp_error_t SpRegisterSocketHALCallbacks(const sp_sockethal_callbacks_t* cbs, void* data);

/*
TODO:
SpPresetUnsubscribe
SpPresetSubscribe(int, void*, unsigned long int)
SpPlayPresetEx(void*, unsigned long int)
SpPlayPreset(int, unsigned int, void*, unsigned long int)
SpConnectionLoginZeroConf(const char*, const char*, const char*, const char*)
*/


#endif
#ifdef __cplusplus
}
#endif

/**
 * Cache file structure
 * uint32_t num1;   		0x000 ? = 1
 * uint32_t datasize; 		0x004 Size of encrypted track data 
 * uint32_t chunksize;		0x008 Size of a chunk in Bitmap = 4116
 * uint32_t num2;			0x00b ? = 60
 * uint32_t num3;			0x010 ? = 0
 * uint32_t num4;			0x014 ? = 0
 * uint8_t blob1[36];		0x018 Maybe some sort of key/non ascii
 * uint8_t blob2[52];		0x03b Blob, filled with 0xcc
 * uint8_t bitmap[0x1400];	Bitmap chunk availablility
 * ...
 */