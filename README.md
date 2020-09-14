# fbpanel

A lightweight gtk2 panel for Linux desktop.

This is a fork of `http://aanatoly.github.io/fbpanel`.

# Changes

- full cmake build and install
    - generate config.h
    - generate po/*.mo files
    - generate www/*.html files
- updated po files
- remove old build scripts

# Install

```
git clone <repo> fbpanel
cd fbpanel
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<prefix> ..
make
make install
```
