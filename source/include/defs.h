#ifndef __DEFS_H__
#define __DEFS_H__

// hard definitions

#include "types.h"
#include "teensy.h"
#include "ccm.h"

//#define SILENT // suppress prints and uart init

#ifndef SILENT
#define DEBUG_UARTN 1 // default debug __teensy__ uartn
#endif

#define DEFAULT_CLOCKSPEED CCM_ARM_CLKF_FULL_SPEED

#endif