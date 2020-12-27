#include <emscripten/val.h>
#include <stdio.h>
#include <math.h>

using namespace emscripten;

void fillBuffer(emscripten::val ev) {
  float data[256];
  for (int i = 0; i < 256; ++i) {
    double x = (double) rand() / RAND_MAX * 2.0 - 1.0; // float in range -1 to 1
    data[i] = (float) x;
  }
  ev["outputBuffer"].call<void>("copyToChannel", 0, 0); // val(data)
}

int main() {
  val ctx   = val::global("AudioContext").new_();
  val proc  = ctx.call<val>("createScriptProcessor", 256, 0, 1);
  // proc.set("onaudioprocess", val(&fillBuffer));
  proc.call<void>("connect", ctx["destination"]);
  printf("All done!\n");
}
