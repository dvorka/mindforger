# ZLib 1.2.3 Windows 64bit port

This is a copy of Windows port of Zlib 1.2.3.

Header files in the `include` folder were taken from [Zlib dev package](http://gnuwin32.sourceforge.net/downlinks/zlib-lib-zip.php).  

`include/zconf.h` was patched to fix compilation on Windows, using the following diff:
```
  289,294c289,291
  < #  if not defined(WINDOWS) && not defined(WIN32)
  < #    include <unistd.h>    /* for SEEK_* and off_t */
  < #    ifdef VMS
  < #      include <unixio.h>   /* for off_t */
  < #    endif
  < #    define z_off_t off_t
  ---
  > #  include <unistd.h>    /* for SEEK_* and off_t */
  > #  ifdef VMS
  > #    include <unixio.h>   /* for off_t */
  295a293
  > #  define z_off_t off_t

```

Binaries in the `lib` folder were taken from [64-bit build of Zlib](http://www.winimage.com/zLibDll/zlib123dllx64.zip) 

