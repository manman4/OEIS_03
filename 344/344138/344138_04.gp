M=61;

a(n) = sum(k=1, n, numdiv(gcd(k, n))^4);
for(n=1, M, print1(a(n), ", "));