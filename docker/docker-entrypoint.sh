#!/bin/sh
[ -z $CONFIG ] && config=Release || config="$CONFIG"

cmake \
    -DTARGET_ARCH=x64 \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=$config \
&&
cmake \
    --build build \
    --config $config \
    --parallel $(nproc)
