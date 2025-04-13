M=19;

\\ a(n) = -Sum_{d|n} phi(n/d) * (-n)^d.
b(n) = -sumdiv(n, d, eulerphi(n/d)*(-n)^d);
for(n=1, M, print1(b(n), ", "));

\\ a(n) = -Sum_{k=1..n} (-n)^gcd(n,k).
d(n) = -sum(k=1, n, (-n)^gcd(n,k));
for(n=1, M, print1(b(n)-d(n), ", "));

\\ Main diagonal of A382994.
a(n, k) = -sumdiv(n, d, eulerphi(n/d)*(-k)^d);
for(n=1, M, print1(b(n)-a(n, n), ", "));  

