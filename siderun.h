#ifndef __SIDERUN_H__
#define __SIDERUN_H__

#define _GNU_SOURCE

#include <time.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <grp.h>
#include <libgen.h>
#include <gelf.h>
#include <libelf.h>
#include <limits.h>
#include <mntent.h>
#include <pwd.h>
#include <sched.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/reboot.h>
#include <unistd.h>

#include "jsfacade.h"

typedef struct {
  int offset;
  int size;
} SectionInfo;

SectionInfo* getSectionInfo(char *targetpath, char *section_name);
char* getCurrentPath();

void initJsArgs(char*, int, int, char *[]);
void initJsLib();
void initEmbed();

#endif
