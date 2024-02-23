M=22;
N=100;

\\ a(n) = (n-1)! * Sum_{d|n} d/(d-1)!.
a(n) = (n-1)! * sumdiv(n, d, d/((d-1)!));
for(n=1, M, print1(a(n), ", "))

\\ If p is prime, a(p) = p + (p-1)!.
forprime(p=2, N, print1(a(p) - (p + (p-1)!), ", "))
