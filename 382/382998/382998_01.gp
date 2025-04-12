M=19;

\\ a(n) = Sum_{d|n} phi(n/d) * (-n)^(d-1).
b(n) = sumdiv(n, d, eulerphi(n/d)*(-n)^(d-1));
for(n=1, M, print1(b(n), ", "));

\\ a(n) = Sum_{k=1..n} (-n)^(gcd(n,k) - 1).
d(n) = sum(k=1, n, (-n)^(gcd(n,k) - 1));
for(n=1, M, print1(b(n)-d(n), ", "));

\\ Main diagonal of A382993.
a(n, k) = -sumdiv(n, d, eulerphi(n/d)*(-k)^d)/n;
for(n=1, M, print1(b(n)-a(n, n), ", "));  

\\ Main diagonal of A382995.
a(n, k) = sumdiv(n, d, eulerphi(n/d)*(-k)^(d-1));
for(n=1, M, print1(b(n)-a(n, n), ", "));  

\\ a(n) = [x^n] Sum_{k>=1} phi(k) * log(1 + n*x^k) / k.
c(n) = polcoef(sum(k=1, n, eulerphi(k) * log(1 + n*x^k + x*O(x^n)) / k), n);
for(n=1, M, print1(b(n)-c(n), ", "));