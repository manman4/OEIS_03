M=23;
N=100;

\\ a(n) = (n-1)! * Sum_{d|n} 1/((d-1)! * (n/d)!^(d-1)).
a(n) = (n-1)! * sumdiv(n, d, 1/((d-1)! * (n/d)!^(d-1)));
for(n=1, M, print1(a(n), ", "))

\\ If p is prime, a(p) = 1 + (p-1)!.
for(n=1, N, if(isprime(n), print1(a(n) - (1 + (n-1)!), ", ")))