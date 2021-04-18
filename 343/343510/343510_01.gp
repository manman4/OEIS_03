T(n, k) = sum(j=1, n, gcd(j, n)^k);
for(n=1, 11, for(k=1, n, print1(T(k, n-k+1), ", ")))