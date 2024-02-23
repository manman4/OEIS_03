M=22;
N=100;

\\ 	a(n) = n! * Sum_{d|n} 1/(d-1)!.
a(n) = n! * sumdiv(n, d, 1/((d-1)!));
for(n=1, M, print1(a(n), ", "))
for(n=1, M, print1(a(n)/n, ", "))

\\ If p is prime, a(p) = p + p!.
forprime(p=2, N, print1(a(p) - (p + p!), ", "))
