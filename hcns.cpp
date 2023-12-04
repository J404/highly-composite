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

// Helper function to print out prime factorization of each number.
void printExponents(vector<int> exponents) {
    for (int i = 0; i < numPrimes; i++) {
        cout << exponents[i] << ",";
    }
}

// Value contains both numerical value & prime factorization exponents for each number.
struct Value {
    uint256_t value;
    vector<int> exponents;
};

// returns s_n, static portion of each number as described by Siano
Value s(Value *n) {
    int numExponents = n -> exponents.size();
    Value result;
    result.value = 1;
    result.exponents = vector<int>();

    for (int i = 0; i < numExponents; i++) {
        if (n -> exponents[i] > 0) {
            result.value *= primes[i];
            result.exponents.push_back(1);
        } else {
            result.exponents.push_back(0);
        }
    }

    return result;
}

// returns v_n, variable portion of each number as described by Siano
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

/**
 * Returns a candidate list for the next highly composite number based on the current HCN n.
 * These candidates must be manually checked afterwards to determine which is the actual next HCN, by 
 * manually calculating the number of divisors of each and seeing the first increase.
*/
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

    // For later, we need these consts
    uint256_t rp = v(n).value / nextLargestPrime;
    uint256_t rm = v(n).value * largestPrime;

    // Iterate through all descending permutations of the variable portion of the number, i.e. change the first F exponents
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
            candidates.push_back(newVal);

            //===============================
            // Optional: double check there wasn't a mistake somewhere when changing the exponents.
            // These errors tend to crop up when we've reached the int storage limit.
            uint256_t test = 1;
            for (int q = 0; q < numPrimes; q++) {
                test *= (uint256_t) pow(primes[q], exponents[q]);
            }

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
                // If we're still >= 0, good to continue
                if (variableExponents[i] >= 0) {
                    break;
                }
            } else {
                // Otherwise, it means we have to go back and decrement first non-zero digit
                if (i == 0 && variableExponents[i] == 0) {
                    looping = false;
                    break;
                } else if (variableExponents[i] > 0) {
                    variableExponents[i]--;
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

// Returns the number of divisors of a number based on the divisor theorem
int numDivisors(Value *n) {
    int result = 1;

    for (int i = 0; i < numPrimes; i++) {
        result *= n -> exponents[i] + 1;
    }

    return result;
}

// Comparator for Value objects based on .value for sorting
bool compareValues(Value a, Value b) {
    return a.value < b.value;
}

int main() {
    Value currHcn;
    currHcn.value = 2;
    currHcn.exponents = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int idealNum = 1000;
    int foundNum = 1;
    int offset = 1;

    vector<Value> candidates = getCandidateList(&currHcn, foundNum);

    // Keep iterating until we've found the 'ideal' number of HCNs or run out of candidates for some reason
    while (foundNum < idealNum && candidates.size() > 0) {
        // Sort list of candidates & calculate # of divisors of current
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