#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_KEEPALIVE
float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("lerp", &lerp);
}

class MyClass {
public:
  EMSCRIPTEN_KEEPALIVE
  MyClass(int x, std::string y)
    : x(x)
    , y(y)
  {}

  EMSCRIPTEN_KEEPALIVE
  void incrementX() {
    ++x;
  }

  EMSCRIPTEN_KEEPALIVE
  int getX() const { return x; }
  
  EMSCRIPTEN_KEEPALIVE
  void setX(int x_) { x = x_; }

  static std::string getStringFromInstance(const MyClass& instance) {
    return instance.y;
  }

private:
  int x;
  std::string y;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<MyClass>("MyClass")
    .constructor<int, std::string>()
    .function("incrementX", &MyClass::incrementX)
    .property("x", &MyClass::getX, &MyClass::setX)
    .class_function("getStringFromInstance", &MyClass::getStringFromInstance)
    ;
}

int main() {
  auto c = MyClass(1, "2");

  EM_ASM({
    var instance = $0;     // apparently that doesn't work
    instance.incrementX(); // instance.incrementX is not a function
    console.log('The value of x is now: ' + instance.x);
  }, &c);

  return 0;
}
