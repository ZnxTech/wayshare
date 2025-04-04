#ifndef WS_WAYSHARE_H
#define WS_WAYSHARE_H

#include <stdint.h>

// +-----------------------+
// | wayshare return codes |
// +-----------------------+

// pre/suf-fix guild:
// ==================
// N_:  no/missing
// NS_: not supported
// _F:  fail/failed

typedef int32_t ecode_t;

#define WS_OK 1

#define WSE_UTIL_FDF            -0x0000	// failed to create the posix shm file.
#define WSE_UTIL_ALLOCF         -0x0001	// failed to allocate the posix shm file.

#define WSE_WL_NDISPLAY         -0x0100	// no display found.
#define WSE_WL_NREGISTRY        -0x0101	// no registry found.
#define WSE_WL_NOUTPUT          -0x0102	// no outputs were found.
#define WSE_WL_NSHM             -0x0103	// no wl shm found.
#define WSE_WL_NXDG_OUTPUT      -0x0104	// no xdg output manager found.
#define WSE_WL_NSCREENSHOT      -0x0105	// no wlr/cosmic screencopying protocol found.
#define WSE_WL_FDF              -0x0106	// failed to obtain the posix shm file fd.
#define WSE_WL_MMAPF            -0x0107	// failed to mmap the posix shm file.
#define WSE_WL_BUFFERF          -0x0108	// failed to create a wl buffer.

#define WSE_IMG_NSFORMAT        -0x0200	// format received is not currently supported by wayshare.
#define WSE_IMG_PARSEF          -0x0201	// failed to parse the data buffer into an image.

#define WSE_POST_CURLF          -0x0400	// curl failed to init.
#define WSE_POST_NUPLOADER      -0x0401	// uploader not found.
#define WSE_POST_NSETTING       -0x0402	// missing critical uploader setting.
#define WSE_POST_REQUESTF       -0x0403	// curl request failed.
#define WSE_POST_RESPONSEF      -0x0404	// curl request resulted in an error/invalid respose.

#define WSE_CONFIG_NHOME        -0x0500	// no standard unix home directory found.
#define WSE_CONFIG_NCONFIG_DIR  -0x0501	// no standard xdg config directory found.
#define WSE_CONFIG_NCONFIG_FILE -0x0502	// no json config file at the wayshare config directory.
#define WSE_CONFIG_JSON_PARSEF  -0x0503	// the json config file failed to parse.
#define WSE_CONFIG_NJSON_OBJECT -0x0504	// missing object inside the json config file.
#define WSE_CONFIG_NVAR_VALUE   -0x0505
#define WSE_CONFIG_NVAR_CLOSE   -0x0506

#define WSE_SELECTOR_NWLR_LAYER -0x0600
#define WSE_SELECTOR_NSELECTION -0x0601

#endif						   // WS_WAYSHARE_H
