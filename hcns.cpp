/**
 * James Bish, jrb327
 * hcns.cpp
 * 11/13/2023
 * Created as a part of MATH303 HCN project in order to determine the first n Highly Composite Numbers.
 * Created in collaboration with Julian Churchill, Joshua Tam.
 * Created in model of Siano's paper, "An Algorithm For Generating Highly Composite Numbers".
 * All code is my own.
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;

const int G = 10;
const int F = 6;

// TODO: dynamically set list of possible primes later, based on F / num size
int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
int numPrimes = 25;

void printExponents(vector<int> exponents) {
    for (int i = 0; i < numPrimes; i++) {
        cout << exponents[i] << ",";
    }
}

struct Value {
    uint256_t value;
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
            result.value *= (uint256_t) pow(primes[i], n -> exponents[i] - 1);
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
vector<Value> getCandidateList(Value *n, int currHcnNum) {
    vector<Value> candidates = vector<Value>();

    uint256_t nextLargestPrime, largestPrime;
    int nextLargestPrimeIndex, largestPrimeIndex;

    // Find the largest prime in the current HCN, and the next largest one, by looping from the 
    // end of the exponents list and finding the first nonzero one
    for (int i = numPrimes - 1; i >= 0; i--) {
        if (n -> exponents[i] >= 1) {
            largestPrime = (uint256_t) primes[i];
            largestPrimeIndex = i;
            nextLargestPrime = primes[i + 1];
            nextLargestPrimeIndex = i + 1;
            break;
        }
    }

    // cout << "Largest primt found is " << largestPrime << "(at index " << largestPrimeIndex << ")" << "for num " << n -> value << endl;

    // For later, we need these consts
    uint256_t rp = v(n).value / nextLargestPrime;
    uint256_t rm = v(n).value * largestPrime;

    // Iterate through combinations of the variable portion of the number, i.e. change the first 4 exponents
    vector<int> variableExponents(F, G);

    bool looping = true;
    while (looping) {
        bool toAdd = false;
        uint256_t actualVal = 1;

        // Determine current variable value from the variable exponents alone
        uint256_t vVal = 1;
        for (unsigned long int i = 0; i < variableExponents.size(); i++) {
            vVal *= (uint256_t) pow(primes[i], variableExponents[i]);
        }
        // uint256_t vVal = (uint256_t) pow(2, i) * (uint256_t) pow(3, j) * (uint256_t) pow(5, k) * (uint256_t) pow(7, l);

        // Determine the prime factorization of this value from the variable exponents and the static portion of the number
        Value sN = s(n);
        vector<int> exponents(numPrimes, 0);

        // For each prime / exponent val, either add the variable & static portion, or just take the static portion
        for (int q = 0; q < numPrimes; q++) {
            if (q < F) {
                exponents[q] = variableExponents[q] + sN.exponents[q]; // could be vN for first val
            } else {
                exponents[q] = sN.exponents[q];
            }
        }
        string status = "a";

        // Now, we must determine if this is an actual valid candidate.
        if (vVal > v(n).value && vVal <= v(n).value * 2) {
            // First, if it's between the 'typical' range we expect, add it as a possibility.
            actualVal = vVal * s(n).value; // True value = v(n) * s(n)
            toAdd = true;
            status = "b";
        } else if (vVal > rp && vVal <= rp * 2) {
            // Next, if it's between the extended range considering it has a larger max prime, add it as a candidate.
            actualVal = vVal * s(n).value * nextLargestPrime;
            exponents[nextLargestPrimeIndex] += 1;
            toAdd = true;
            status = "c";
        } else if (vVal > rm && vVal <= rm * 2) {
            // old / good:
            // actualVal = vVal * s(n).value / previousLargestPrime;
            // exponents[previousLargestPrimeIndex] -= 1;
            // Finally, if it's between the extended range considering it has a smaller max prime, add it as a candidate.
            actualVal = vVal * s(n).value / largestPrime;
            exponents[largestPrimeIndex] -= 1;
            toAdd = true;
            status = "d";
        }

        // If any of the above conditions were met, add this candidate!
        if (toAdd) {
            Value newVal;
            newVal.value = actualVal;
            newVal.exponents = exponents;

            //===============================
            // Optional: double check there wasn't a mistake somewhere when changing the exponents
            uint256_t test = 1;
            for (int q = 0; q < numPrimes; q++) {
                test *= (uint256_t) pow(primes[q], exponents[q]);
            }
            candidates.push_back(newVal);

            if (newVal.value != test) {
                cout << "Discrepancy detected" << endl;
                cout << newVal.value << " vs " << test << endl;
                cout << status << endl;
                cout << vVal << " * " << s(n).value << " * " << nextLargestPrime << " = " << vVal * s(n).value * nextLargestPrime;
                exit(1);
            }
            //===============================
        }

        // Get next permutation of exponents
        for (long unsigned int i = F - 1; i >= 0; i--) {
            if (i == F - 1) {
                // Decrement rightmost digit
                variableExponents[i]--;
                // cout << "Decrementing #" << i << " from " << variableExponents[i] + 1 << " to " << variableExponents[i] << endl;
                // If we're still >= 0, good to continue
                if (variableExponents[i] >= 0) {
                    // cout << "It's still +, good to stop." << endl;
                    break;
                }
            } else {
                // Otherwise, it means we have to go back and decrement first non-zero digit
                // cout << "Looping back b/c -, @ " << i << " now." << endl;
                if (i == 0 && variableExponents[i] == 0) {
                    looping = false;
                    break;
                } else if (variableExponents[i] > 0) {
                    // cout << "@ " << i << " found " << variableExponents[i] << " > 0." << endl;
                    variableExponents[i]--;
                    // cout << "Decremented & now " << variableExponents[i] << endl;
                    for (int j = i; j < F; j++) {
                        variableExponents[j] = variableExponents[i];
                    }
                    break;
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

bool compareValues(Value a, Value b) {
    return a.value < b.value;
}

int main() {
    Value currHcn;
    // currHcn.value = 27720;
    // currHcn.exponents = {3, 2, 1, 1, 1, 0, 0, 0, 0, 0};
    currHcn.value = 2;
    currHcn.exponents = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int idealNum = 1000;
    int foundNum = 1;
    int offset = 1;

    // uint256_t = (uint256_t) pow(2, 8)

    vector<Value> candidates = getCandidateList(&currHcn, foundNum);

    while (foundNum < idealNum && candidates.size() > 0) {
        sort(candidates.begin(), candidates.end(), compareValues);
        int origNumDivisors = numDivisors(&currHcn);

        
        for (long unsigned int i = 0; i < candidates.size(); i++) {
            Value candidate = candidates[i];
            if (numDivisors(&candidate) > origNumDivisors) {
                cout << "HCN " << foundNum + offset << ": " << candidate.value << ". Prime powers: ";
                printExponents(candidate.exponents);
                cout << endl;
                currHcn = candidate;
                candidates = getCandidateList(&currHcn, foundNum);
                foundNum++;
                break;
            }
        }
    }

    exit(0);
}