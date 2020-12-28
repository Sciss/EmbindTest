#include <emscripten/bind.h>
#include <emscripten.h>
#include <iostream>

class AudioDriver {
public:
  AudioDriver() {
    printf("AudioDriver: constructor.\n");
    if (AudioDriver::instance != NULL) {
      throw std::runtime_error("AudioDriver can only have one instance\n");
    }
    AudioDriver::instance = this;
  }

  bool setup() {
    int res = EM_ASM_INT({
      var AudioContext = window.AudioContext || window.webkitAudioContext;
      if (!AudioContext) return -1;
      if (!Module.audioDriver) Module.audioDriver = {};
      var ad = Module.audioDriver;
      ad.context = new AudioContext();
      return 0;
    });
    return res == 0;
  }

  void doSomething() {
    printf("Doing something\n");
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
    .function("doSomething", &AudioDriver::doSomething);
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

//  EM_ASM({
//    Module.audio_driver().doSomething();
//  });
  printf("Done.\n");
  return 0;
}
