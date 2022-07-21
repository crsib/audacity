#!/usr/bin/env bash

imageTag=ghcr.io/audacity/crossbuild-focal-arm64:latest

scriptLocation=$(dirname "$(readlink -f "$0")")
audacityLocation=$(readlink -f "$scriptLocation/../../..")

if [ $# -eq 1 ]; then
    if [ $1 = "--build-image" ]; then
        pushd $scriptLocation
        docker build -t $imageTag .
        popd
    else
        docker pull $imageTag
    fi
else
    docker pull $imageTag
fi

buildDir="$audacityLocation/.build.cross"

docker_run() {
    docker run --rm -ti \
        --volume=$audacityLocation:/audacity \
        --volume=$buildDir:/build:rw \
        -e BUILDER_UID="$( id -u )" \
        -e BUILDER_GID="$( id -g )" \
        -e BUILDER_USER="$( id -un )" \
        -e BUILDER_GROUP="$( id -gn )" \
        $imageTag \
        "$@"
}

docker_run_cross() {
    docker run --rm -ti \
        --volume=$audacityLocation:/audacity \
        --volume=$buildDir:/build:rw \
        -e BUILDER_UID="$( id -u )" \
        -e BUILDER_GID="$( id -g )" \
        -e BUILDER_USER="$( id -un )" \
        -e BUILDER_GROUP="$( id -gn )" \
        -e CROSS_ARCH="aarch64" \
        $imageTag \
        "$@"
}

# Build image-compiler first
#docker_run cmake -S /audacity -B /build/amd64 -DCMAKE_BUILD_TYPE=RelWithDebInfo
#docker_run cmake --build /build/amd64 --target image-compiler -- -j`nproc`

# Build aarch64

#docker_run cmake -S /audacity -B /build/arm64 \
#    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
#    -DCMAKE_TOOLCHAIN_FILE=/audacity/linux/arm/arm64/toolchain.cmake \
#    -DIMAGE_COMPILER_EXECUTABLE=/build/amd64/utils/image-compiler
#
#docker_run cmake --build /build/arm64 -- -j`nproc`

# Build linuxdeploy for aarch64

#if [ ! -d "$buildDir/linuxdeploy" ]; then
#    git clone --recurse-submodule https://github.com/linuxdeploy/linuxdeploy $buildDir/linuxdeploy
#fi
#
#docker_run cmake -S /build/linuxdeploy -B /build/linuxdeploy/.build \
#                 -DCMAKE_TOOLCHAIN_FILE=/audacity/linux/arm/arm64/toolchain.cmake \
#                 -DCMAKE_BUILD_TYPE=Release \
#                 -DUSE_CCACHE=Off \
#                 -DBUILD_TESTING=Off
#
#docker_run cmake --build /build/linuxdeploy/.build -- -j`nproc`

# Trick create_appimage.sh to use our linuxdeploy
mkdir -p $buildDir/arm64/linuxdeploy
cp -v $buildDir/linuxdeploy/.build/bin/* $buildDir/arm64/linuxdeploy

# Package AppImage
#docker_run_cross cmake --build /build/arm64 --target package -- -j`nproc`
docker_run_cross cmake --install /build/arm64 --prefix /build/package
