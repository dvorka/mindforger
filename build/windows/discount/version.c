#include "config.h"

char markdown_version[] = BRANCH VERSION
#if 4 != 4
		" TAB=4"
#endif
#if USE_AMALLOC
		" DEBUG"
#endif
		;
