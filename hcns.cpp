#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// TODO: make constants more settable / customizable
const int G = 8;
const int F = 4;

// TODO: dynamically set list of possible primes later, based on F / num size
int primes[] = {2, 3, 5, 7, 11, 13, 17, 19};

struct Value {
    long unsigned int value;
    vector<int> exponents;
};

long unsigned int s(Value *n) {
    long unsigned int result = 1;
    int numExponents = n -> exponents.size();

    for (int i = 0; i < numExponents; i++) {
        if (n -> exponents[i] > 0) {
            result *= primes[i];
        }
    }

    return result;
}

long unsigned int v(Value *n) {
    long unsigned int result = 1;

    for (int i = 0; i < F; i++) {
        if (n -> exponents[i] > 1) {
            result *= pow(primes[i], n -> exponents[i] - 1);
        }
    }

    return result;
}

int main() {
    Value hcn85;
    hcn85.value = 97772875200;
    hcn85.exponents = {6, 3, 2, 2, 1, 1, 1, 1};

    cout << hcn85.value << " " << hcn85.exponents[0] << " " << hcn85.exponents[1] << endl;
    cout << s(&hcn85) << endl;
    cout << v(&hcn85) << endl;
    exit(0);
}