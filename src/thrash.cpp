#include <iostream>
#include <chrono>

template <typename T> struct element {
    T message;
    chrono::steady_clock::time_point time;
};

int main()
{
    struct element e;


}