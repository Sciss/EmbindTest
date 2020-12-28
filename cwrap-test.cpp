#include <emscripten.h>

extern "C" {
  double SquareVal(double val) {
    return val * val;
  }
}

int main() {
  EM_ASM({
    SquareVal = Module.cwrap('SquareVal', 'number', ['number']);
    var x = 12.5;
    alert('Computing: ' + x + ' * ' + x + ' = ' + SquareVal(x));
  });
}
