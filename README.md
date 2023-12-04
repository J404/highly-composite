# Highly Composite Number Project

This project is completed under Professor Singer's MATH303 course to compute and discover the first N highly composite numbers. A _highly composite number_ is defined as a number with more divisors than any number smaller than it. Highly composite numbers are usually difficult to find, since (1) they become increasingly sparsingly spaced on the number line, and (2) computing the number of divisors of a number requires the prime factorization of a number, which is difficult to compute for large numbers. Additionally, the only way to check for the next highly composite number is to go through all feasible possibilities and manually check them.  

Algorithms have been made to speed up this process and make finding the next HCN feasible. This project implements [Siano's Algorithm](https://wwwhomes.uni-bielefeld.de/achim/julianmanuscript3.pdf), with some modifications [1], to limit the number of HCN candidates and thus improve the search speed.  

The algorithm's source code itself is located in [hcns.cpp](/hcns.cpp).  

[1]: Due to either a misunderstanding of mine of Siano's paper or a mistake in his paper, the case for when the next HCN has a smaller largest prime than the current HCN is a bit different than he describes. Put briefly, instead of dividing by the previous smaller prime, we must divide by the _current_ largest prime in HCN n. Professor Singer: this is expanded more upon in our supplemental paper.  

## Dependencies
Requires Boost multiprecision library in order to store the larger HCNs.  

## Execution  
Running `make` will create an executable, which can be run with `./hcns`.  

## Known Issues
For some reason, the program stalls after the 473 HCN (which is accurate). There are no error messages, and I ran out of time to debug the reason for why this is. I suspect it is due to poor memory management on my side (I'm new to C++), but I can't be sure.  