# EmbindTest

Step by step getting to setting up a web audio script processor node whose audio processing function is a function of a C++ class.

Resources:

- [Embind - Using `val` to transliterate JavaScript to C++](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c)
- [Issue - adding example to Embind documentation](https://github.com/emscripten-core/emscripten/issues/5082)
- [emscripten-discuss - Passing TypedArrays to C/C++ Functions](https://groups.google.com/g/emscripten-discuss/c/CMfYljLWMvY)
- [Gist - how to pass an array of floating point numbers from JavaScript to a C callback](https://gist.github.com/aknuds1/533f7b228aa46e9ee4c8)
- [emscripten-examples - passing arrays](https://github.com/Planeshifter/emscripten-examples/tree/master/01_PassingArrays)
- [C++ to WebAssembly using Bazel and Emscripten](https://hackernoon.com/c-to-webassembly-using-bazel-and-emscripten-4him3ymc)
- [Singleton pattern](https://groups.google.com/g/emscripten-discuss/c/MimQol7peuQ)
- [Ideas why exported symbols might not work, keep alive](https://groups.google.com/g/emscripten-discuss/c/Wkq0lDi76eg)
- [Issue - An efficient way to pass TypedArrays with Embind](https://github.com/emscripten-core/emscripten/issues/5519)

## Emscripten preparation

One needs to install [emscripten](https://emscripten.org), see [download instructions](https://emscripten.org/docs/getting_started/downloads.html).
What worked for me is the installation described there:

    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh

## Plain C oscillator example

    emcc -O2 -Wall -Werror --bind -o oscillator.html oscillator.cpp
    python -m SimpleHTTPServer &
    xdg-open http://0.0.0.0:8000/

## `cwrap` test

    emcc -o cwrap-test.html -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["cwrap"]' -s EXPORTED_FUNCTIONS='["_SquareVal", "_main"]' cwrap-test.cpp

## Plain C script-processor-node

Not yet working!

    emcc -O2 -Wall -Werror --bind -o static_proc.html static_proc.cpp

## Creating Float32 array in JS

We cannot create `Float32Array` in C++ and pass it to JS, but we can do the opposite: Allocate the array
in JS _on the wasm heap_, and pass a pointer to a C++ callback:

    emcc -O1 --bind -o pass_array.html --js-library pass_array_library.js pass_array.cpp

## Instantiating class from JS

Basically the example from the Embind documentation:

    emcc -O1 --bind -o classes.html --post-js classes_post.cpp classes.cpp

Make sure JS code runs inside `addOnPostRun` (see https://github.com/emscripten-core/emscripten/issues/13116 )

## Singleton pattern

__This works__

    emcc --bind -o singleton.html singleton.cpp

I suspect this will be the only reliable way to patch C++/JS together. We can use `EM_ASM` to set up the script processor node, and then the JS `onaudioprocess` function
in turn calls into the C++ code, having a pointer to `_malloc` allocated wasm memory that is wrapped by an `Float32Array`.
