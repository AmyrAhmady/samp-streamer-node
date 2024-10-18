#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"

cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=$config \
    -DTARGET_ARCH=x64 \
&&
cmake \
    --build build \
    --config $config \
    --parallel $(nproc)
