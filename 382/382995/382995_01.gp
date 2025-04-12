M=11;

\\ A(n,k) = Sum_{d|n} phi(n/d) * (-k)^(d-1).
a(n, k) = sumdiv(n, d, eulerphi(n/d)*(-k)^(d-1));
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", ")));  

\\ A(n,k) = -(1/n) * Sum_{j=1..n} (-k)^gcd(n,j).
e(n, k) = -sum(j=1, n, (-k)^gcd(n,j))/n;
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)-e(k, n-k+1), ", ")));  

\\ G.f. of column k: Sum_{j>=1} phi(j) * log(1 + k*x^j) / j.
b(n, k) = polcoef(sum(j=1, n, eulerphi(j) * log(1 + k*x^j + x*O(x^n)) / j), n);
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)-b(k, n-k+1), ", ")));  

