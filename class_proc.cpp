// author: Hanns Holger Rutz
// CC0 - public domain

#include <emscripten/bind.h>
#include <emscripten.h>
#include <iostream>

class AudioDriver {
private:
  int numChannels = 0;
  int bufSize     = 0;
  float* bufPtr;

public:
  AudioDriver() {
    printf("AudioDriver: constructor.\n");
    if (AudioDriver::instance != NULL) {
      throw std::runtime_error("AudioDriver can only have one instance\n");
    }
    AudioDriver::instance = this;
  }

  ~AudioDriver() {
    printf("AudioDriver: destructor.\n");
    bufPtr = NULL;
    AudioDriver::instance = NULL;
    EM_ASM({
      if (Module.audioDriver) {
        if (ad.context) {
          ad.context.close();
        }
        if (ad.bufPtr) {
          Module._free(ad.bufPtr);
        }
        Module.audioDriver = undefined;
      }
    });
  }

  void init_buffer(int numChannels, int bufSize, uintptr_t bufPtr) {
    printf("AudioDriver: init_buffer.\n");
    this->numChannels  = numChannels;
    this->bufSize      = bufSize;
    // cf. https://stackoverflow.com/questions/20355880/#27364643
    this->bufPtr       = reinterpret_cast<float*>(bufPtr);
  }

  bool setup() {
    printf("AudioDriver: setup.\n");
    int res = EM_ASM_INT({
      var AudioContext = window.AudioContext || window.webkitAudioContext;
      if (!AudioContext) return -1;
      if (!Module.audioDriver) Module.audioDriver = {};
      var ad          = Module.audioDriver;
      ad.context      = new AudioContext();
      ad.numChannels  = 2;
      // using zero buffer size gives us default buffer size;
      // we currently use zero inputs.
      ad.proc         = ad.context.createScriptProcessor(0, 0, ad.numChannels);
      ad.bufSize      = ad.proc.bufferSize;
      var numSamples  = ad.numChannels * ad.bufSize;
      var numBytes    = numSamples * Float32Array.BYTES_PER_ELEMENT;
      ad.bufPtr       = Module._malloc(numBytes);
      ad.floatBuf     = [];
      for (var ch = 0; ch < ad.numChannels; ch++) {
        ad.floatBuf[ch] = new Float32Array(Module.HEAPU8.buffer, 
          ad.bufPtr + (ch * ad.bufSize * Float32Array.BYTES_PER_ELEMENT), numSamples);
      }
      var self        = Module.audio_driver();
      self.init_buffer(ad.numChannels, ad.bufSize, ad.bufPtr);
      ad.proc.onaudioprocess = function(e) {
        self.process();
        var bOut    = e.outputBuffer;
        var bProc   = ad.floatBuf;
        var numCh   = Math.min(bOut.numberOfChannels, ad.numChannels);
        var bufSize = ad.bufSize;
        for (var ch = 0; ch < numCh; ch++) {
          bOut.copyToChannel(bProc[ch], ch, 0);
        }
      };
      return 0;
    });
    return res == 0;
  }

  bool start() {
    printf("AudioDriver: start.\n");
    printf("numChannels %d, bufSize %d.\n", numChannels, bufSize);
    if (bufSize == 0) return false;

    int res = EM_ASM_INT({
      var ad = Module.audioDriver;
      if (!ad) return -1;
      
      ad.proc.connect(ad.context.destination);

      return 0;
    });
    return res == 0;
  }

  bool stop() {
    printf("AudioDriver: stop.\n");
    
    int res = EM_ASM_INT({
      var ad = Module.audioDriver;
      if (!ad) return -1;

      ad.proc.disconnect(ad.context.destination);

      return 0;
    });
    return res == 0;
  }

  void process() {
    // printf("AudioDriver: process.\n");
    float *b = bufPtr;
    int numCh = numChannels;
    int n     = bufSize;
    int i     = 0;
    for (int ch = 0; ch < numCh; ch++) {
      for (int stop = i + n; i < stop; i++ ) {
        float r = emscripten_random();
        b[i] = r * 0.4f - 0.2f; // -14 dBFS
      } 
    }
  }

  static AudioDriver* instance;
};

AudioDriver* AudioDriver::instance = NULL;

extern "C" AudioDriver* audio_driver() {
  if (AudioDriver::instance == NULL) {
    new AudioDriver;
  }
  return AudioDriver::instance;
}

EMSCRIPTEN_BINDINGS(Audio_Driver) {
  emscripten::class_<AudioDriver>("AudioDriver")
    .function("start"       , &AudioDriver::start   )
    .function("stop"        , &AudioDriver::stop    )
    .function("process"     , &AudioDriver::process )
    .function("init_buffer" , &AudioDriver::init_buffer, emscripten::allow_raw_pointers())
    ;
  emscripten::function("audio_driver", &audio_driver, emscripten::allow_raw_pointers());
}

// to be able to see runtime exception text in the browser
extern "C" void EMSCRIPTEN_KEEPALIVE what_to_stderr(intptr_t pointer) {
  auto error = reinterpret_cast<std::runtime_error *>(pointer);
  std::cerr << error->what() << "\n";
}

int main() {
  // set up exception error printing function
  EM_ASM({
      window.onerror = function(message, url, line, column, e) {
        if (typeof e != 'number') return;
        var pointer = e;
        Module.ccall('what_to_stderr', 'number', ['number'], [pointer]);
      }
  });

  auto d = new AudioDriver;
  bool okSetup = d->setup();
  if (!okSetup) {
    printf("ERROR: Could not setup driver.\n");
    exit(1);
  }

  bool okStart = d->start();
  if (!okStart) {
    printf("ERROR: Could not start audio processing.\n");
    exit(1);
  }

  // d->stop();

  printf("Done.\n");
  return 0;
}
