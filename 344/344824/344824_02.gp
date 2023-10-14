M=11;

T(n, k) = sum(j=1, n, sumdiv(j, d, (-k)^(d-1)));
for(n=1, M, for(k=1, n, print1(T(k, n-k), ", ")));