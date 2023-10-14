M=11;

T(n, k) = sum(j=1, n, k^(gcd(j, n)-1));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));