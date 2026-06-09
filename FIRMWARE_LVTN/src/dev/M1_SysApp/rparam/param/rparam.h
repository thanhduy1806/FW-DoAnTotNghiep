#ifndef GS_PARAM_REMOTE_H
#define GS_PARAM_REMOTE_H
/* Copyright (c) 2013-2018 GomSpace A/S. All rights reserved. */

#include <param/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   Magic checksum.
   If specified, the rparam server will skip checksum validation.
*/
#define GS_RPARAM_MAGIC_CHECKSUM    0x0bb0

#ifndef GS_CSP_PORT_RPARAM
#define GS_CSP_PORT_RPARAM          7
#endif

#ifdef __cplusplus
}
#endif

#endif // GS_PARAM_REMOTE_H