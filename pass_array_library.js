// "use strict";

var LibraryTst = {
  initialize: function (callback) {
    callback = getFuncWrapper(callback, 'vi')
    // callback = Module.getFuncWrapper(callback, 'vi')
    // callback = addFunction(callback, 'vi')
    var numBytes = 2 * Float32Array.BYTES_PER_ELEMENT
    var ptr = Module._malloc(numBytes)

    try {
      callback(ptr)

      console.log('Callback filled buffer like so:')
      var i = 0
      for (i = 0; i < 2; ++i) {
        console.log(HEAPF32[(ptr/Float32Array.BYTES_PER_ELEMENT)+i])
      }
    }
    finally {
      Module._free(ptr)
    }
  },
  initialize__deps: ['$getFuncWrapper'],
}

mergeInto(LibraryManager.library, LibraryTst)
