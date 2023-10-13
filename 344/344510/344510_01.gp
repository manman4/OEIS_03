M=52;

a(n) = sum(k=1, n, k*gcd(k, n));
for(n=1, M, print1(a(n), ", "));