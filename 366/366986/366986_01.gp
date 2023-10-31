T(n, k) = sumdiv(n, d, binomial(d+k-1, k));
for(n=1, 12, for(k=1, n, print1(T(k, n-k), ", ")));
