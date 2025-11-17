#ifndef _X11_XLIB_H_
#define _X11_XLIB_H_

#include "X.h"

typedef struct _XDisplay Display;
typedef struct {
    int x, y;
    unsigned int width, height;
    unsigned int border_width;
    int depth;
} XWindowAttributes;

#endif