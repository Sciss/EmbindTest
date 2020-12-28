#!/bin/bash
emcc --bind -o class_proc.js -s DISABLE_EXCEPTION_CATCHING=0 -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall","callMain"]' --pre-js class_proc-pre.js class_proc.cpp

