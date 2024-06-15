\\ G.f. of column k: 1/(1 - x * (1 + x^2)^k).
T(n, k) = polcoef(1/(1 - x * (1 + x^2)^k + x*O(x^(n+10))), n);
for(n=0, 12, for(k=0, n, print1(T(k, n-k),", "))) 
