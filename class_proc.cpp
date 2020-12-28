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
    EM_ASM({
      if (Module.audioDriver) {
        if (ad.context) {
          ad.context.close();
        }
        Module.audioDriver = undefined;
      }
    });
    AudioDriver::instance = NULL;
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
      ad.proc         = ad.context.createScriptProcessor(0, 0, ad.numChannels);
      ad.bufSize      = ad.proc.bufferSize;
      var numSamples  = ad.numChannels * ad.bufSize;
      var self        = Module.audio_driver();
      self.init_buffer(ad.numChannels, ad.bufSize, 0);
      ad.proc.onaudioprocess = function(e) {
        self.process();
      };
      return 0;
    });
    return res == 0;
  }

  bool start() {
    printf("AudioDriver: start.\n");
    printf("numChannels %d, bufSize %d.\n", numChannels, bufSize);
    if (bufSize == 0) return false;

    return true;
  }

  bool stop() {
    printf("AudioDriver: stop.\n");
    return true;
  }

  void process() {
    printf("AudioDriver: process.\n");
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
  auto ok = d->setup();
  if (!ok) {
    printf("ERROR: Could not setup driver.\n");
    exit(1);
  }

  d->start();
  d->stop();

  printf("Done.\n");
  return 0;
}
