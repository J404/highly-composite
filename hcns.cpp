#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// TODO: make constants more settable / customizable
const int G = 8;
const int F = 4;

// TODO: dynamically set list of possible primes later, based on F / num size
int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
int numPrimes = 10;

struct Value {
    long unsigned int value;
    vector<int> exponents;
};

Value s(Value *n) {
    int numExponents = n -> exponents.size();
    Value result;
    result.value = 1;
    result.exponents = vector<int>();

    for (int i = 0; i < numExponents; i++) {
        if (n -> exponents[i] > 0) {
            result.value *= primes[i];
            // result.exponents[i] = 1;
            result.exponents.push_back(1);
        } else {
            result.exponents.push_back(0);
        }
    }

    return result;
}

Value v(Value *n) {
    Value result;
    result.value = 1;
    result.exponents = vector<int>();

    for (int i = 0; i < F; i++) {
        if (n -> exponents[i] > 1) {
            result.value *= pow(primes[i], n -> exponents[i] - 1);
            result.exponents.push_back(n -> exponents[i] - 1);
        } else {
            result.exponents.push_back(0);
        }
    }

    for (int j = 0; j < numPrimes - F; j++) {
        result.exponents.push_back(0);
    }

    return result;
}

// Note: n is assumed to be a HCN
vector<Value> getCandidateList(Value *n) {
    // TODO: for now, exponents are hard coded. Make them variable later.
    vector<Value> candidates = vector<Value>();

    unsigned long int nextLargestPrime, largestPrime;
    int nextLargestPrimeIndex, largestPrimeIndex;
    // find largest prime of n
    for (int i = numPrimes - 1; i >= 0; i--) {
        if (n -> exponents[i] >= 1) {
            largestPrime = primes[i];
            largestPrimeIndex = i;
            nextLargestPrime = primes[i + 1];
            nextLargestPrimeIndex = i + 1;
            break;
        }
    }

    unsigned long int rp = v(n).value / nextLargestPrime;
    unsigned long int rm = v(n).value * largestPrime;

    for (int i = G; i >= 0; --i) {
        for (int j = i; j >= 0; --j) {
            for (int k = j; k >= 0; --k) {
                for (int l = k; l >= 0; --l) {
                    unsigned long int vVal = pow(2, i) * pow(3, j) * pow(5, k) * pow(7, l);
                    bool toAdd = false;
                    unsigned long int actualVal = 1;
                    int variableExponents[F] = {i, j, k, l};
                    Value sN = s(n);
                    vector<int> exponents = vector<int>(numPrimes, 0);

                    for (int q = 0; q < numPrimes; q++) {
                        if (q < F) {
                            exponents[q] = variableExponents[q] + sN.exponents[q]; // could be vN for first val
                        } else {
                            exponents[q] = sN.exponents[q];
                        }
                    }

                    if (vVal > v(n).value && vVal <= v(n).value * 2) {
                        actualVal = vVal * s(n).value;
                        toAdd = true;
                    } else if (vVal > rp && vVal <= rp * 2) {
                        actualVal = vVal * s(n).value * nextLargestPrime;
                        exponents[nextLargestPrimeIndex] += 1;
                        toAdd = true;
                    } else if (vVal > rm && vVal <= rm * 2) {
                        // old / good:
                        // actualVal = vVal * s(n).value / previousLargestPrime;
                        // exponents[previousLargestPrimeIndex] -= 1;
                        actualVal = vVal * s(n).value / largestPrime;
                        exponents[largestPrimeIndex] -= 1;
                        toAdd = true;
                    }

                    if (toAdd) {
                        Value newVal;
                        newVal.value = actualVal;
                        newVal.exponents = exponents;

                        // cout << "exponent list was: ";
                        long unsigned int test = 1;
                        for (int q = 0; q < numPrimes; q++) {
                            // cout << exponents[q] << ", ";
                            test *= pow(primes[q], exponents[q]);
                        }
                        candidates.push_back(newVal);

                        if (newVal.value != test) {
                            exit(1);
                        }
                    }
                }
            }
        }
    }

    return candidates;
}

int numDivisors(Value *n) {
    int result = 1;

    for (int i = 0; i < numPrimes; i++) {
        result *= n -> exponents[i] + 1;
    }

    return result;
}

void printExponents(vector<int> exponents) {
    for (int i = 0; i < numPrimes; i++) {
        cout << exponents[i] << ", ";
    }
}

bool compareValues(Value a, Value b) {
    return a.value < b.value;
}

int main() {
    Value currHcn;
    // currHcn.value = 27720;
    // currHcn.exponents = {3, 2, 1, 1, 1, 0, 0, 0, 0, 0};
    currHcn.value = 2;
    currHcn.exponents = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int idealNum = 100;
    int foundNum = 1;
    int offset = 1;

    vector<Value> candidates = getCandidateList(&currHcn);

    while (foundNum < idealNum && candidates.size() > 0) {
        sort(candidates.begin(), candidates.end(), compareValues);
        int origNumDivisors = numDivisors(&currHcn);

        
        for (long unsigned int i = 0; i < candidates.size(); i++) {
            Value candidate = candidates[i];
            if (numDivisors(&candidate) > origNumDivisors) {
                cout << foundNum + offset + 1 << ": " << candidate.value << endl;
                printExponents(candidate.exponents);
                cout << endl;
                currHcn = candidate;
                candidates = getCandidateList(&currHcn);
                foundNum++;
                break;
            }
        }
    }

    exit(0);
}