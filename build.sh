#!/bin/bash

set -e

EXT_DIR=$(pwd)

git clone https://github.com/alliedmodders/sourcemod --branch $BRANCH --single-branch "$EXT_DIR/sourcemod-$BRANCH"

# Boost
wget https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.7z
7z x boost_1_73_0.7z > /dev/null
7z x boost_stage.7z -o boost_1_73_0

mkdir -p "$EXT_DIR/build"
pushd "$EXT_DIR/build"
python3 "$EXT_DIR/configure.py" --enable-optimize --sm-path "$EXT_DIR/sourcemod-$BRANCH"
ambuild

# might be optional
strip package/addons/sourcemod/extensions/socket.ext.so

popd