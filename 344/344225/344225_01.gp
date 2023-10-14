M=67;

a(n) = sum(k=1, n, numdiv(gcd(k, n)^(n/gcd(k, n))));
for(n=1, M, print1(a(n), ", "));