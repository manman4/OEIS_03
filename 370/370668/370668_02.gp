M=22;

\\ a(n) = Sum_{d|n} d! * binomial(d,n/d-1).
a(n) = sumdiv(n, d, d!*binomial(d, n/d-1));
for(n=1, M, print1(a(n), ", "))

\\ If p is an odd prime, a(p) = p!.
forprime(p=3, M, print1(a(p)-p!, ", "))