#include <emscripten/val.h>
#include <stdio.h>
#include <math.h>

using namespace emscripten;

void fillBuffer(emscripten::val ev) { // ok? 
  float data[256];
  // void* ptr = &data; // ok?

  for (int i = 0; i < 256; ++i) {
    // float in range -1 to 1
    double x = (double) rand() / RAND_MAX * 2.0 - 1.0;
    data[i] = (float) x;
  }
  // ev["outputBuffer"].call<void>("copyToChannel", ptr, 0, 0); // ok?
}

int main() {
  val ctx   = val::global("AudioContext").new_();
  val proc  = ctx.call<val>("createScriptProcessor", 256, 0, 1);
  void *ptr = (void *) &fillBuffer;
  proc.set("onaudioprocess", ptr);
  proc.call<void>("connect", ctx["destination"]);
  printf("All done!\n");
}
