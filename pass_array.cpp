#include <cstdio>
#include <emscripten.h>

extern "C" {
    extern void initialize(void (*callback)(float*));
}

// EM_ASM ({
//   var callback = Runtime.getFuncWrapper($0, 'vi')
// })

void callback(float* output)
{
    for (int i = 0; i < 2; ++i)
    {
        output[i] = i+1;
    }
    printf("Callback was invoked\n");
}

int main()
{
    initialize(&callback);
    return 0;
}

