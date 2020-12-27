# EmbindTest

Step by step getting to setting up a web audio script processor node whose audio processing function is a function of a C++ class.

Resources:

- [Embind - Using `val` to transliterate JavaScript to C++](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c)
- [Issue - adding example to Embind documentation](https://github.com/emscripten-core/emscripten/issues/5082)

## Plain C oscillator example

    emcc -O2 -Wall -Werror --bind -o oscillator.html oscillator.cpp
    python -m SimpleHTTPServer &
    xdg-open http://0.0.0.0:8000/

## Plain C script-processor-node

Not yet working!

    emcc -O2 -Wall -Werror --bind -o static_proc.html static_proc.cpp


