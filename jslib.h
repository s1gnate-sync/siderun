#ifndef __JSLIB_H__
#define __JSLIB_H__

#include "siderun.h"

void initFiles();
void initProcess();
void initOs();
void initFs();

#ifdef __JSLIB__
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char quiet;

#define regGlobalFunction(name) global_setFunction(#name, name##_, 0);
#define regFunction(name) object_setFunction(#name, name##_, 0);
#define printError(context, message) { if (!quiet) fprintf(stderr, "%s: %s\n", context, message); }
#define printLastError(context) printError(context, strerror(errno))

#endif
