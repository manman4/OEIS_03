M=20;

a(n) = sum(k=1, n, gcd(k, n)^(gcd(k, n)-2));
for(n=1, M, print1(a(n), ", "));