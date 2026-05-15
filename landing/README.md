# Landing

Real estate data scraper using curl-impersonate for browser-like HTTP requests.

## Prerequisites

- Linux (tested on Ubuntu/Debian)
- [vcpkg](https://github.com/microsoft/vcpkg) installed with `VCPKG_ROOT` set

```bash
mkdir -p ~/tools
cd ~/tools
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```

## Setup

```bash
# Install dependencies and build curl-impersonate
./scripts/setup.sh

# Configure the CMake build
./scripts/configure.sh

# Build
cmake --build build
```

## Run

```bash
./build/landing
```