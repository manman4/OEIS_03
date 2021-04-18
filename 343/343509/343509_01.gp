M=26;

a(n) = sum(k=1, n, gcd(k, n)^7);
for(n=1, M, print1(a(n),", ")); 