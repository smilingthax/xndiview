# NDI® Viewer for X11 (xcb)

- Only video, no audio support (yet).
- Requires NDI SDK from http://ndi.tv/.
- libxcb and libxcb-shm are the only external dependencies.

Usage:
```
  xndiview [-l | -h | [-pmvgf] ndi_source]

  -l: List available sources

  -p: Send PVW tally
  -m: Send PGM tally
  -v: Verbose
  -g: Gray background
  -f: Fullscreen
```

Copyright (c) 2021 Tobias Hoffmann

License: https://opensource.org/licenses/MIT

Includes portions of libyuv (https://chromium.googlesource.com/libyuv/libyuv), see libyuv/LICENSE.  
Also includes a packaged (and slightly extended) version of xcbcpp (https://github.com/smilingthax/xcbcpp).

NDI® is a registered trademark of NewTek, Inc.

