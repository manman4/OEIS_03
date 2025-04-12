M=11;

\\ A(n,k) = -(1/n) * Sum_{d|n} mu(n/d) * (-k)^d.
a(n, k) = -sumdiv(n, d, moebius(n/d)*(-k)^d)/n;
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", ")));  