M=23;

\\ G.f. of column k: 1 / (1 - x - x^2 - x^3)^(k+1).
a(n, k) = polcoef(1/(1 - x - x^2 - x^3 + x*O(x^n))^(k+1), n);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));



