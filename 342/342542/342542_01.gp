M=60;

a(n) = sum(k=1, n, eulerphi(gcd(k, n))^(n/gcd(k, n)-1));
for(n=1, M, print1(a(n), ", "));