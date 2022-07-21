set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
set(CMAKE_STRIP /usr/bin/aarch64-linux-gnu-strip CACHE FILEPATH "" FORCE)
set(CMAKE_OBJCOPY /usr/bin/aarch64-linux-gnu-objcopy)
set(CMAKE_LINKER /usr/bin/aarch64-linux-gnu-ld)

set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

set(CMAKE_CROSSCOMPILING_EMULATOR "qemu-aarch64-static;-L;/usr/aarch64-linux-gnu/" CACHE FILEPATH "Path to the emulator for the target system.")
set(PKG_CONFIG_EXECUTABLE "/usr/bin/aarch64-linux-gnu-pkg-config" CACHE FILEPATH "pkg-config executable")
