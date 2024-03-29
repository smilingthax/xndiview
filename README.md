# NDI® Viewer for X11 (xcb)

- Only video, no audio support (yet).
- Requires NDI SDK from http://ndi.tv/.
- libxcb and libxcb-shm are the only external dependencies.

Usage:
```
  xndiview [-l | -h | [-pmvgfit] ndi_source]

  -l  List available sources
  -h  Help

  -p  Send Preview Tally
  -m  Send Program Tally
  -v  Verbose
  -g  Gray letterbox background
  -f  Fullscreen
  -i  Treat ndi_source as ip:port instead of ndi name
  -t  Show transparency

```

Known issues:
- Keyboard handling uses keycode directly instead of using (e.g.) xkbcommon to map it first to keysym.
- Does not support other visuals than TrueColor 24bpp with "BGRX" layout (red_mask = 0xff0000, green_mask = 0x00ff00, blue_mask = 0x0000ff).
- No support for alpha / indication of transparency.

Copyright (c) 2022 Tobias Hoffmann

License: https://opensource.org/licenses/MIT

Includes portions of libyuv (https://chromium.googlesource.com/libyuv/libyuv), see libyuv/LICENSE.  
Also includes a packaged (and slightly extended) version of xcbcpp (https://github.com/smilingthax/xcbcpp).

NDI® is a registered trademark of NewTek, Inc.

