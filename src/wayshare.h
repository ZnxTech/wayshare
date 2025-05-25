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

typedef uint32_t ecode_t;

#define WS_OK 0

#define WSE_UTIL_FDF            0x0100	// failed to create the posix shm file.
#define WSE_UTIL_ALLOCF         0x0101	// failed to allocate the posix shm file.

#define WSE_WL_NDISPLAY         0x0200	// no display found.
#define WSE_WL_NREGISTRY        0x0201	// no registry found.
#define WSE_WL_NOUTPUT          0x0202	// no outputs were found.
#define WSE_WL_NSHM             0x0203	// no wl shm found.
#define WSE_WL_NXDG_OUTPUT      0x0204	// no xdg output manager found.
#define WSE_WL_NSCREENSHOT      0x0205	// no wlr/cosmic screencopying protocol found.
#define WSE_WL_FDF              0x0206	// failed to obtain the posix shm file fd.
#define WSE_WL_MMAPF            0x0207	// failed to mmap the posix shm file.
#define WSE_WL_BUFFERF          0x0208	// failed to create a wl buffer.

#define WSE_IMG_NSFORMAT        0x0300	// format received is not currently supported by wayshare.
#define WSE_IMG_REFORMATF       0x0301	// failed to parse the data buffer into an image.

#define WSE_POST_CURLF          0x0500	// curl failed to init.
#define WSE_POST_NUPLOADER      0x0501	// uploader not found.
#define WSE_POST_NSETTING       0x0502	// missing critical uploader setting.
#define WSE_POST_REQUESTF       0x0503	// curl request failed.
#define WSE_POST_RESPONSEF      0x0504	// curl request resulted in an error/invalid respose.

#define WSE_CONFIG_NHOME        0x0600	// no standard unix home directory found.
#define WSE_CONFIG_CONFIG_FILEF 0x0601	// failed to retrive json config file.
#define WSE_CONFIG_NJSON_OBJECT 0x0602	// missing object inside the json config file.
#define WSE_CONFIG_JSON_PATHF   0x0602	// json path is invalid and failed to parse.
#define WSE_CONFIG_NVAR_VALUE   0x0604  // missing value for variable string variable.
#define WSE_CONFIG_NVAR_CLOSE   0x0605  // variable string variable missing closing '$'.

#define WSE_SELECTOR_NWLR_LAYER 0x0700
#define WSE_SELECTOR_NSELECTION 0x0701

#endif						   // WS_WAYSHARE_H
