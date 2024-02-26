M=22;

\\ a(n) = Sum_{d|n} (-1)^(d-1) * (d+n/d-1)!/(d-1)!.
a(n) = sumdiv(n, d, (-1)^(d-1) * (d+n/d-1)!/(d-1)!);
for(n=1, M, print1(a(n), ", "))

\\ If p is an odd prime, a(p) = p + p!.
forprime(p=3, 100, print1(a(p)-(p+p!), ", "))