M=10;

\\ A(n,k) = -Sum_{d|n} phi(n/d) * (-k)^d.
a(n, k) = -sumdiv(n, d, eulerphi(n/d)*(-k)^d);
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", ")));  

\\ A(n,k) = -Sum_{j=1..n} (-k)^gcd(n,j).
e(n, k) = -sum(j=1, n, (-k)^gcd(n,j));
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)-e(k, n-k+1), ", ")));  

\\ G.f. of column k: k * Sum_{j>=1} phi(j) * x^j / (1 + k*x^j).
b(n, k) = polcoef(k * sum(j=1, n, eulerphi(j) * x^j / (1 + k*x^j + x*O(x^n))), n);
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)-b(k, n-k+1), ", ")));  

