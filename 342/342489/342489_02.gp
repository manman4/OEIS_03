M=50;

a(n) = sum(k=1, n, eulerphi(n/gcd(k, n))^(n/gcd(k, n)-2));
for(n=1, M, print1(a(n), ", "));