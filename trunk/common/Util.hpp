#ifndef __DAMARIS_UTIL_H
#define __DAMARIS_UTIL_H

#include <stdio.h>

#define LOG(m) fprintf(stderr,"LOG from %s:%d: %s",__FILE__,__LINE__,m);

#define LOGF(m,...) fprintf(stderr,"LOG from %s:%d: ",__FILE__,__LINE__); \
		      fprintf(stderr,m,__VA_ARGS__);

#endif
