M=29;
N=100;

\\ a(n) = n! * Sum_{d|n} 1/((d-1)! * (n/d)!^d).
a(n) = n! * sumdiv(n, d, 1/((d-1)! * (n/d)!^d));
for(n=1, M, print1(a(n), ", "))

\\ If p is prime, a(p) = 1 + p.
forprime(p=2, N, print1(a(p) - (1 + p), ", "))
