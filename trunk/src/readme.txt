
### 1. About ffdshow video decoder ###

ffdshow is an open source directShow filter and VFW codec that can be used for the fast and high-quality decoding of a large number of audio and video formats.

ffdshow has a large number of postprocessing filters that can optionally be enabled to enhance audio and video output. Examples of these filters are deblocking, resizing, aspect ratio correction, sharpness, subtitle display, deinterlacing, cropping, and color correction. 

### 2. Features ###

- Fast video decompression using optimized MMX, SSE, SSE2, SSSE3, and 3DNow! SIMD instructions.
- Support for a large number of video formats: DivX, Xvid, H.264/AVC, Theora, MPEG1/2, VP3/5/6, FLV1/4, SVQ1/3, and many many more.
- Hardware deinterlacing support for RAW video.
- Can act as generic postprocessing filter for processing the raw ouput of other decoders.
- Video postprocessing for higher quality video playback. 
- Automatic quality control: automatically reduces postprocessing level when CPU load is high.
- Hue, saturation and luminance correction (MMX optimized).
- Tray icon with menu and quick access to configuration dialog.
- Multithreaded resize: faster resize on multi-core CPUs.
- Subtitles support.
- Blacklist/whitelist functionality for configurating ffdshow to be used only by specific applications. Useful for solving compatibilty issues.
- Completely free software: ffdshow is distributed under GPL

### 3. ffdshow-tryouts ###

ffdshow was originally developed by Milan Cutka. Since Milan Cutka stopped updating in 2006, we launched a new project called 'ffdshow-tryouts'.

### 4. Web links ###

ffdshow:
http://sourceforge.net/projects/ffdshow-tryout/

Xvid:
http://www.xvid.org/

Libav:
http://www.libav.org/

FFmpeg:
http://www.ffmpeg.org/

libmpeg2:
http://libmpeg2.sourceforge.net/

xsharpen, unsharp mask, msharpen, hue and saturation code
http://neuron2.net/

Doom9:
http://forum.doom9.org/showthread.php?t=120465

or Wikipedia, the free encyclopedia:
http://en.wikipedia.org/wiki/Ffdshow

### 5. Copying ###

ffdshow tryouts is distributed under GPLv2. Boost C++ Libraries and threadpool are distributed under Boost Software License. Other used sources except of CPU utilization detection routine are distributed under GPL or LGPL.
