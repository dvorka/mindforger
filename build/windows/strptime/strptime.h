#ifndef STRPTIME_H
#define STRPTIME_H

#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif

char   *strptime(const char *, const char *, struct tm *);

#ifdef __cplusplus
}
#endif

#endif //STRPTIME_H
