#!/bin/bash

# Set up environment variables for Homebrew LLVM
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib/c++ -L/opt/homebrew/opt/llvm/lib/unwind -lunwind"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
export SDKROOT="/Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk"

# Compile the program with additional flags
clang++ -std=c++14 \
        -isysroot "$SDKROOT" \
        -I"$SDKROOT/usr/include" \
        -I"/opt/homebrew/opt/llvm/include" \
        -L"/opt/homebrew/opt/llvm/lib" \
        src/main.cc -o src/main 