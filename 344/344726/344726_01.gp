M=11;

T(n, k) = sum(j=1, n, (-1)^(j+1)*(n\j)^k);
for(n=1, M, for(k=1, n, print1(T(k, n-k+1), ", ")));