#include <cstdio>

extern "C" {
    void initialize(void (*callback)(float*));
}

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

