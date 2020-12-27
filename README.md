# EmbindTest

Step by step getting to setting up a web audio script processor node whose audio processing function is a function of a C++ class.

Resources:

- [Embind - Using `val` to transliterate JavaScript to C++](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c)
- [Issue - adding example to Embind documentation](https://github.com/emscripten-core/emscripten/issues/5082)
- [emscripten-discuss - Passing TypedArrays to C/C++ Functions](https://groups.google.com/g/emscripten-discuss/c/CMfYljLWMvY)
- [Gist - how to pass an array of floating point numbers from JavaScript to a C callback](https://gist.github.com/aknuds1/533f7b228aa46e9ee4c8)
- [emscripten-examples - passing arrays](https://github.com/Planeshifter/emscripten-examples/tree/master/01_PassingArrays)

## Plain C oscillator example

    emcc -O2 -Wall -Werror --bind -o oscillator.html oscillator.cpp
    python -m SimpleHTTPServer &
    xdg-open http://0.0.0.0:8000/

## Plain C script-processor-node

Not yet working!

    emcc -O2 -Wall -Werror --bind -o static_proc.html static_proc.cpp

## Creating Float32 array in JS

Not yet working!

    emcc -O1 -Wall -Werror --bind -o pass_array.html --js-library pass_array_library.js pass_array.cpp

