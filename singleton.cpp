#include <emscripten/bind.h>
#include <emscripten.h>

class MyClass {
public:
  MyClass() {}

  void doSomething() {
    printf("Doing something\n");
  }
};

static MyClass* singleton_val;

MyClass* singleton() {
  if (singleton_val == NULL) {
    singleton_val = new MyClass;
  }
  return singleton_val;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::class_<MyClass>("MyClass")
    .function("doSomething", &MyClass::doSomething);
  function("singleton", &singleton, emscripten::allow_raw_pointers());
}

int main() {
  EM_ASM({
    Module.singleton().doSomething();
  });
  return 0;
}
