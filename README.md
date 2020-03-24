bus1-api
========

Bindings to the Linux Bus1 Syscall API

The bus1-api project provides bindings to the linux kernel bus1 syscall API. It
provides an ISO-C11 API to access bus1 functionality, hiding the exact details
involved in calling the correct kernel functions.

### Project

 * **Website**: <https://bus1.github.io/bus1-api>
 * **Bug Tracker**: <https://github.com/bus1/bus1-api/issues>

### Requirements

The requirements for this project are:

 * `libc` (e.g., `glibc >= 2.16`)

At build-time, the following software is required:

 * `meson >= 0.41`
 * `pkg-config >= 0.29`

### Build

The meson build-system is used for this project. Consult upstream
documentation for detailed help. In most situations the following
commands are sufficient to build and install from source:

```sh
mkdir build
cd build
meson setup ..
ninja
meson test
ninja install
```

No custom configuration options are available.

### Repository:

 - **web**:   <https://github.com/bus1/bus1-api>
 - **https**: `https://github.com/bus1/bus1-api.git`
 - **ssh**:   `git@github.com:bus1/bus1-api.git`

### License:

 - **Apache-2.0** OR **LGPL-2.1-or-later**
 - See AUTHORS file for details.
