#include <cstdlib>
#include <csignal>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "spotify.h"
#include "login_data.h"

#define CONCAT(a, b) a ## b

static void memdump(const void* data, size_t len) {
	auto fmt = std::clog.flags();
	const uint8_t* ptr = (const uint8_t*)data;
	for(size_t i=0; i < len; i += 16) {
		std::clog << std::endl << std::hex << std::setw(3) << std::setfill('0') << i << "   ";
		for(int x = 0; x != 16; x++)
			if(i + x < len) std::clog << " " << std::hex << std::setw(2) << std::setfill('0') << (int)ptr[i+x];
			else std::clog << "   ";
		std::clog << " | ";
		for(int x = 0; i + x < len && x != 16; x++) {
			if(isprint(ptr[i+x]))
				std::clog << (char)ptr[i+x];
			else std::clog << '.';
		}
	}
	std::clog << std::endl;
	std::clog.flags(fmt);
}

static bool isloggedin = false;

inline bool check_return(sp_error_t e) {
	const char* str;
	switch(e) {
		case E_OK: str = NULL; break;
		case E_API_VERSION: str = "api version missmatch"; break;
		case E_NULL_ARGUMENT: str = "parameter null"; break;
		case E_INVALID_ARGUMENT: str = "parameter out of range"; break;
		case E_UNINITIALIZED: str = "init not called"; break;
		case E_ALREADY_INITIALIZED: str = "init already called"; break;
		default: str = "Unknown return code"; break;
	}
	if(str) std::clog << "Returncode: " << str << " (" << (int)e << ")" << std::endl;
	return !str;
}

template<typename T>
inline void clean(T& ptr) {
	memset(&ptr, 0x00, sizeof(T));
}

static uint8_t app_key[] = {
	0x01, 0x42, 0xdf, 0x0e, 0xa6, 0x68, 0x2a, 0x1c, 0xba, 0x50, 0xa4, 0x5a, 0x4e, 0x62, 0x6f, 0xc3,
	0x13, 0x87, 0x71, 0xa7, 0x08, 0x89, 0x0f, 0x73, 0x99, 0x94, 0xef, 0x0a, 0x50, 0xaf, 0x78, 0xa8,
	0x0d, 0xe9, 0x6d, 0xd5, 0x4a, 0xc2, 0x5c, 0xa5, 0x6e, 0xbf, 0x1f, 0x98, 0x2b, 0xda, 0x94, 0xa0,
	0x08, 0xf0, 0xdb, 0xd6, 0x67, 0x85, 0x5c, 0xaf, 0xe8, 0xd6, 0xc2, 0x8b, 0xe1, 0xd2, 0x42, 0x8f,
	0xc8, 0x9e, 0xf7, 0x50, 0xd8, 0xdc, 0x34, 0x5c, 0x90, 0xaf, 0x04, 0x9b, 0x3c, 0x6d, 0x71, 0x30,
	0x23, 0x52, 0xa3, 0x80, 0x15, 0xca, 0x09, 0xb8, 0x46, 0x86, 0x7a, 0x91, 0x8e, 0xc0, 0x1f, 0x3e,
	0x99, 0x09, 0x0a, 0xbf, 0x98, 0x6e, 0x5f, 0x0f, 0xcf, 0xea, 0xa2, 0x78, 0xd2, 0x5e, 0x09, 0xea,
	0x8a, 0x51, 0x30, 0x97, 0xe1, 0x0d, 0xbc, 0xad, 0x47, 0xfb, 0x90, 0x60, 0x66, 0x39, 0xe2, 0x1c,
	0xaa, 0xb1, 0x59, 0xfd, 0xe3, 0x09, 0x30, 0x5a, 0x7a, 0xd7, 0xa2, 0x35, 0x51, 0xc6, 0x3a, 0x00,
	0x75, 0xee, 0xc3, 0x76, 0x98, 0x75, 0x3e, 0x02, 0xf3, 0x40, 0x50, 0x21, 0x7c, 0x51, 0x75, 0x9c,
	0xe6, 0x66, 0xd3, 0x31, 0xfe, 0x15, 0x84, 0x35, 0xef, 0x45, 0x2e, 0x9c, 0x7d, 0xd0, 0xc5, 0x16,
	0xea, 0xbe, 0x71, 0x07, 0x7a, 0x6c, 0x40, 0x26, 0x75, 0xef, 0x90, 0x33, 0xa7, 0x65, 0xca, 0xf1,
	0xa6, 0x50, 0x91, 0x6c, 0x99, 0xf4, 0xba, 0x8a, 0x5e, 0xd1, 0xb4, 0x21, 0x9d, 0x81, 0xbb, 0x61,
	0x9a, 0x31, 0xe1, 0x82, 0xad, 0x10, 0xd7, 0xd9, 0x06, 0xe0, 0xe3, 0x1f, 0x2f, 0xcf, 0x21, 0x91,
	0x57, 0x05, 0xf6, 0x39, 0x9f, 0x4c, 0x1f, 0xbd, 0x0b, 0x68, 0x6e, 0xcb, 0x10, 0xd9, 0x6b, 0xc4,
	0xbb, 0x76, 0xdc, 0x47, 0x96, 0x3e, 0xd4, 0xc6, 0xaf, 0xb5, 0x80, 0x4c, 0x0d, 0xb5, 0x63, 0x82,
	0xc1, 0x96, 0x35, 0xc1, 0x82, 0x65, 0x62, 0x53, 0x0a, 0xce, 0xb7, 0x6e, 0xf8, 0x1e, 0xf6, 0x8b,
	0x66, 0xfa, 0x5e, 0xc1, 0x49, 0x7e, 0xc2, 0x95, 0x51, 0xe3, 0xfa, 0x30, 0x55, 0x23, 0x79, 0x69,
	0x33, 0x3b, 0xe6, 0xb2, 0x07, 0xac, 0x41, 0x6a, 0x5a, 0xc4, 0x05, 0x8c, 0x4a, 0xfc, 0xd2, 0x44,
	0x8f, 0xac, 0x92, 0x77, 0xc6, 0x76, 0xbe, 0x68, 0x4b, 0xcc, 0x17, 0x3a, 0xdd, 0x8d, 0x40, 0xd5,
	0x77
};


int main(int argc, const char** argv) {
	assert(sizeof(sp_zeroconfvars_t) == 428);
	assert(sizeof(sp_init_config_t) == 0x90);
	assert(sizeof(app_key) == 321);
	// Setup debug output
	{
		sp_debug_callbacks_t dcbs;
		dcbs.print = [](const char* line, void* context) {
			std::clog << line << std::endl;
		};
		check_return(SpRegisterDebugCallbacks(&dcbs, NULL));
	}

	sp_init_config_t cfg;
	clean(cfg);
	cfg.version = SP_APIVERSION;
	cfg.wmem_size = 0x1000000;
	cfg.wmem = malloc(cfg.wmem_size);
	cfg.app_key = app_key;
	cfg.app_key_len = sizeof(app_key);
	cfg.uniqueid = "fd7ccecc5c988df3";
	cfg.displayname = "Spotify_Android_SDK_Demo";
	cfg.brand = "UNKNOWN";
	cfg.model = "anbox";
	cfg.clientid = "089d841ccc194c10a77afad9e1c11d54";
	cfg.osversion = "7.1.1_x86_64";
	cfg.devicetype = DT_SMARTPHONE;
	cfg.on_error = [](sp_error_t e, void* data) { std::clog << "=>async_error(" << (int)e << ", " << data << ")" << std::endl; };
	cfg.on_error_context = (void*)0xDEADBEEF;

	if(!check_return(SpInit(&cfg))) {
		std::clog << "Init failed, exiting" << std::endl;
		return -1;
	}

	if(1) {
		sp_playback_callbacks_t cbs;
		clean(cbs);
		cbs.onNotify = [](sp_playbacknotify_t n, void* data) -> int{
			std::clog << "=>playback.onNotify(" << (int)n << ", " << data << ")" << std::endl;
			if(n == PN_METADATACHANGED) {
				char buf[128];
				sp_metadata_t meta;
				check_return(SpGetMetadata(&meta, 0));
				check_return(SpGetMetadataImageURL(meta.image_uri, buf, 128));
				std::clog << "Artist:   " << meta.artist_name << " (" << meta.artist_uri << ")" << std::endl;
				std::clog << "Album:    " << meta.album_name << " (" << meta.album_uri << ")" << std::endl;
				std::clog << "Track:    " << meta.track_title << " (" << meta.track_uri << ")" << std::endl;
				std::clog << "Options:  " << meta.duration << "ms, "<<meta.bitrate << "k, idx=" << meta.playlist_idx << ", idx2=" << meta.arg3 << std::endl;
				std::clog << "Image url:" << buf << std::endl;
			}
			return 0;
		};
		cbs.onAudioData = [](const short* frames, unsigned long nframes, const sp_sampleformat_t* format, unsigned int arg4, void* handle) { return nframes; };
		cbs.onSeek = [](uint64_t position, void* data) { std::clog << "=>playback.onSeek(" << position << ", " << data << ")" << std::endl; };
		cbs.onApplyVolume = [](unsigned short vol, void* data) { std::clog  << "=>playback.onApplyVolume(" << vol << "," << data << ")" << std::endl; };
		cbs.onUnavailableTrack = [](const char* uri, void* data) { std::clog << std::hex << "=>playback.onUnavailableTrack(" << uri << ", " << data << ")" << std::endl;};
		//cbs.fn6 = [](long a, long b, long c, long d) { std::ios::fmtflags fmt = std::clog.flags(); std::clog << std::hex << "=>playback.fn6(" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl; std::clog.flags(fmt); };;
		check_return(SpRegisterPlaybackCallbacks(&cbs, (void*)0xDEADBEEF));
	}
	if(1) {
		sp_connection_callbacks_t cbs;
		clean(cbs);
		cbs.onNotifyLoggedIn = [](const char* blob, const char* uname, void* handle){
			std::ios::fmtflags fmt = std::clog.flags();
			std::clog  << "=>connection.onNotifyLoggedIn(" << blob << "," << uname << "," << handle << ")" << std::endl;
			std::clog.flags(fmt);
			isloggedin = true;
			sp_zeroconfvars_t zconf;
			clean(zconf);
			check_return(SpZeroConfGetVars(&zconf));
			std::clog << "Token:       " << zconf.token << std::endl;
			std::clog << "Hash:        " << zconf.uniqueid_hash << std::endl;
			std::clog << "Username:    " << zconf.username << std::endl;
			std::clog << "Displayname: " << zconf.displayname << std::endl;
			std::clog << "Accounttype: " << zconf.accounttype << std::endl;
			std::clog << "Devicetype:  " << zconf.devicetype << std::endl;
			std::clog << "Version:     " << zconf.version << std::endl;
			memdump(&zconf, sizeof(zconf));
		};
		cbs.onNotify = [](sp_con_state_t n, void* data){ std::clog << "=>connection.onNotify(" << (int)n << ", " << data << ")" << std::endl; };;
		check_return(SpRegisterConnectionCallbacks(&cbs, (void*)0xDEADBEEF));
	}
	if(0) {
		sp_content_callbacks_t cbs;
		clean(cbs);
		cbs.fn1 = [](const char* uri, long arg1, void* data) { std::clog << "=>content.fn1(" << uri << ", " << arg1 << ", " << data << ")" << std::endl; };
		cbs.fn2 = [](const char* key, const char* uri, long c, void* data) { std::ios::fmtflags fmt = std::clog.flags(); std::clog << std::hex << "=>content.fn2(" << key << ", " << uri << ", " << c << ", " << data << ")" << std::endl; std::clog.flags(fmt); };
		cbs.fn3 = [](long a, long b, long c, long d) { std::ios::fmtflags fmt = std::clog.flags(); std::clog << std::hex << "=>content.fn3(" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl; std::clog.flags(fmt); };
		check_return(SpRegisterContentCallbacks(&cbs, (void*)0xDEADBEEF));
	}
	if(0) {
		sp_storage_callbacks_t cbs;
		clean(cbs);
		cbs.alloc = [](const char* key, unsigned int size, void* data) -> long {
			std::clog << "=>storage.alloc("<<key<<","<<size<<","<<data<<")" << std::endl;
			std::ofstream file(std::string("tmp/") + key, std::ios::binary | std::ios::trunc);
			if(file) {
				char buf[1024];
				clean(buf);
				while(size > 0) {
					auto s = std::min(size, 1024u);
					file.write(buf, s);
					size -= s;
				}
				file.flush();
				file.close();
				return size;
			}
			return 0;
		};
		cbs.write = [](const char* key, unsigned int offset, const void* buf, unsigned int size, void* data) -> long {
			std::clog << "=>storage.write(" << key << ", " << offset << ", " << buf << ", " << size << ", " << data << ")" << std::endl;
			std::fstream file(std::string("tmp/") + key, std::ios::binary | std::ios::in | std::ios::out);
			if(file) {
				file.seekp(offset);
				file.write((char*)buf, size);
				file.flush();
				file.close();
				return size;
			}
			return 0;
		};
		cbs.read = [](const char* key, unsigned int offset, void* buf, unsigned int size, void* data) -> long {
			std::clog << "=>storage.read(" << key << ", " << offset << ", " << buf << ", " << size << ", " << data << ")" << std::endl;
			std::ifstream file(std::string("tmp/") + key, std::ios::binary);
			if(file) {
				file.seekg(offset);
				return file.read((char*)buf, size).gcount();
			}
			return 0;
		};
		cbs.close = [](const char* key, void* data) {
			std::clog  << "=>storage.finish(" << key << ", " << data << ")" << std::endl;
		};
		cbs.fn5 = [](long a, long b, long c, long d) { std::ios::fmtflags fmt = std::clog.flags(); std::clog << std::hex << "=>storage.fn4(" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl; std::clog.flags(fmt); };
		check_return(SpRegisterStorageCallbacks(&cbs, (void*)0xDEADBEEF));
	}
	if(0) {
		sp_prefetch_callbacks_t cbs;
		clean(cbs);
		cbs.fn = [](const char* a, long b, long c, void* d) { std::clog << "=>prefetch.fn(" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl; };
		check_return(SpRegisterPrefetchCallbacks(&cbs, (void*)0xDEADBEEF));
	}
	if(0) {
		sp_dnshal_callbacks_t cbs;
		clean(cbs);
		cbs.lookup = [](const char* host, struct sockaddr* b, void* data) {
			std::clog << "=>dnshal.lookup(" << host << ", " << b << ", " << data << ")" << std::endl;
			memset(b, 0x0, 16);
			struct addrinfo* info;
			if(getaddrinfo(host, NULL, NULL, &info) != 0)
				return -10001;
			*b = *info->ai_addr;
			freeaddrinfo(info);
			// For some reason the first 4 bytes need to be zero
			memset(b, 0x00, 4);
			return 0;
		};
		check_return(SpRegisterDnsHALCallbacks(&cbs, (void*)0xDEADBEEF));
	}

	// wrong login => -112
	check_return(SpConnectionLoginPassword(SP_USER, SP_PASSWORD));

	bool loggedin = false;
	while(true) {
		check_return(SpPumpEvents());
		if(!loggedin && isloggedin) {
			loggedin = true;
			SpPlayUri("spotify:user:sollunad:playlist:7sZWboj9zudtQQLOWLKFXF", 28, 170000);

			if(!check_return(SpPlaybackEnableShuffle(0))) {
				std::clog << "Failed to disable shuffle" << std::endl;
				return -1;
			}
		}
	}
	check_return(SpFree());
}
