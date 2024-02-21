M=11;

\\ G.f. of column k: Sum_{j>=1} j! * x^(j+k-1) / (1+x^k)^(j+1).
T(n, k) = my(x='x+O('x^(n+2))); polcoef( sum(j=1, n, j!*x^(j+k-1)/(1+x^k)^(j+1)), n);

for(n=1, M, for(k=1, n, print1(T(n,k),", ")))