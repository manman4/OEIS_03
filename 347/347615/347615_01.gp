M=9;

T(n, k) = numbpart(n^k);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));