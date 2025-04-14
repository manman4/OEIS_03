M=11;

\\ G.f. of column k: Sum_{j>=1} mu(j) * log(1 + x^j/(1 - k*x^j)) / j.
a(n, k)= polcoef(sum(j=1, n, moebius(j) * log(1 + x^j/(1 - k*x^j) + x*O(x^n)) / j), n);
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", "))); 

