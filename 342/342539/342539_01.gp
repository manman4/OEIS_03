M=30;

a(n) = sum(k=1, n, eulerphi(gcd(k, n))^n);
for(n=1, M, print1(a(n), ", "));