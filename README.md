# Custom YouTube SeekBar

An implementation of SeekBar mimicing one from YouTube using C++.
## Stack

* C++23
* SFML 2.6.x
* GoogleTest
* CMake
  
## Supported features

* Play/Pause/Restart button
* Loading state (for initial 3 seconds)
* Chapters
* Drag-andn-Drop to change the current time
* Left/Right arrow keys to jump forward or backward
  
## To build and run

```shell
cmake -S . -B build
cd build
cmake --build .
./seekbar
```
