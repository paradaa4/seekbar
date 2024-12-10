# Custom YouTube SeekBar

An implementation of SeekBar mimicing one from YouTube using C++.

![image](https://github.com/user-attachments/assets/c11aa00d-9538-4863-ad33-8edc0bdc3b23)

## Stack

* C++23
* SFML 2.6.x
* GoogleTest
* CMake
  
## Supported features

* Play/Pause/Restart button
* Loading state (for initial 3 seconds)
* Chapters
* Showing current time
* Drag-and-Drop to change the current time
* Left/Right arrow keys to jump +/- 10s
* Spacebar key for playing/pausing

## Tested compilers

* clang 15
* g++ 13.1

## Dependencies
From SMFL official documentation:
> On Windows and macOS, all the required dependencies are provided alongside SFML so you won't have to download/install anything else. Building will work out of the box.
>
> On Linux however, nothing is provided. SFML relies on you to install all of its dependencies on your own. Here is a list of what you need to install before building SFML:
> * freetype
> * x11
> * xrandr
> * udev
> * opengl
> * flac
> * ogg
> * vorbis
> * vorbisenc
> * vorbisfile
> * openal
> * pthread
>
> On Ubuntu or other Debian-based OSes, that can be done with the commands below.
> ```shell
> sudo apt update
> sudo apt install \
>     libxrandr-dev \
>     libxcursor-dev \
>     libudev-dev \
>     libopenal-dev \
>     libflac-dev \
>     libvorbis-dev \
>     libgl1-mesa-dev \
>     libegl1-mesa-dev \
>     libdrm-dev \
>     libgbm-dev
> ```

## To build and run

```shell
cmake -S . -B build
cd build
cmake --build .
./seekbar
```
