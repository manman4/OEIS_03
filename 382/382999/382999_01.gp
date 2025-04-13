M=33;

\\ a(n) = Sum_{d|n} phi(n/d) * (-2)^(d-1).
a(n) = sumdiv(n, d, eulerphi(n/d)*(-2)^(d-1));
for(k=1, M, print1(a(k), ", "));    

\\ a(n) = Sum_{k=1..n} (-2)^(gcd(n,k) - 1).
e(n) = sum(k=1, n, (-2)^(gcd(n,k) - 1));
for(k=1, 50, print1(a(k)-e(k), ", "));  



