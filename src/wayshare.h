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

// wayshare ok:
// ============
#define WS_OK 1

// wayshare errors:                      // category:           // description:
// ================                      // =========           // ============
#define WSE_WL_NDISPLAY         -0x0000  // wayland             // no display found.
#define WSE_WL_NREGISTRY        -0x0001  // =======             // no registry found.
#define WSE_WL_NOUTPUT          -0x0002                         // no outputs were found.
#define WSE_WL_NSHM             -0x0003                         // no wl shm found.
#define WSE_WL_NXDG_OUTPUT      -0x0004                         // no xdg output manager found.
#define WSE_WL_NSCREENSHOT      -0x0005                         // no wlr/cosmic screencopying protocol found.
#define WSE_WL_FDF              -0x0006                         // failed to obtain the posix shm file fd.
#define WSE_WL_MMAPF            -0x0007                         // failed to mmap the posix shm file.
#define WSE_WL_BUFFERF          -0x0008                         // failed to create a wl buffer.

#define WSE_SHM_FDF             -0x0100  // posix shared memory // failed to create the posix shm file.
#define WSE_SHM_ALLOCF          -0x0101  // =================== // failed to allocate the posix shm file.

#define WSE_IMG_NSFORMAT        -0x0200  // wayshare raw image  // format received is not currently supported by wayshare.
#define WSE_IMG_PARSEF          -0x0201  // ==================  // failed to parse the data buffer into an image.

#define WSE_CONFIG_NHOME        -0x0500  // config file         // no standard unix home directory found.
#define WSE_CONFIG_NCONFIG_DIR  -0x0501  // ===========         // no standard xdg config directory found.
#define WSE_CONFIG_NCONFIG_FILE -0x0502                         // no json config file at the wayshare config directory.
#define WSE_CONFIG_JSON_PARSEF  -0x0503                         // the json config file failed to parse.
#define WSE_CONFIG_NJSON_OBJECT -0x0504                         // missing object inside the json config file.
#define WSE_CONFIG_NVAR_VALUE   -0x0505
#define WSE_CONFIG_NVAR_CLOSE   -0x0506

#endif // WS_WAYSHARE_H