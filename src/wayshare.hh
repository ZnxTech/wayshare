#ifndef WS_WAYSHARE_H
#define WS_WAYSHARE_H

#include <cstdint>

// +-----------------------+
// | wayshare return codes |
// +-----------------------+

// pre/suf-fix guild:
// ==================
// N_:  no/missing
// NS_: not supported
// _F:  fail/failed

typedef int32_t ws_code_t;

// wayshare ok:
// ============
#define WS_OK 0

// wayshare errors:                 // category:            // description:
// ================                 // =========            // ============
#define WSE_WL_NDISPLAY    -0x0101  // wayland              // no display found.
#define WSE_WL_NREGISTRY   -0x0102  // =======              // no registry found.
#define WSE_WL_NOUTPUT     -0x0103                          // no outputs were found.
#define WSE_WL_NSHM        -0x0104                          // no wl shm found.
#define WSE_WL_NXDG_OUTPUT -0x0105                          // no xdg output manager found.
#define WSE_WL_NSCREENSHOT -0x0106                          // no wlr/cosmic screencopying protocol found.
#define WSE_WL_FDF         -0x0107                          // failed to obtain the posix shm file fd.
#define WSE_WL_MMAPF       -0x0108                          // failed to mmap the posix shm file.
#define WSE_WL_BUFFERF     -0x0109                          // failed to create a wl buffer.

#define WSE_SHM_FDF        -0x0201  // posix shared memory  // failed to create the posix shm file.
#define WSE_SHM_ALLOCF     -0x0202  // ===================  // failed to allocate the posix shm file.

#endif // WS_WAYSHARE_H