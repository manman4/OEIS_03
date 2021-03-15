M=60;

a(n) = sum(k=1, n, eulerphi(gcd(k, n))^2);
for(n=1, M, print1(a(n), ", "));