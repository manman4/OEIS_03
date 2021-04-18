M=45;

a(n) = sum(k=1, n, gcd(k, n)^3);
for(n=1, M, print1(a(n),", "));  