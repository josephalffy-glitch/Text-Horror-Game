#ifndef _XRANDR_H_
#define _XRANDR_H_

#include "../Xlib.h"

typedef XID RROutput;
typedef XID RRCrtc;
typedef XID RRMode;

typedef struct {
    int width, height;
    int mwidth, mheight;
} XRRScreenSize;

#endif