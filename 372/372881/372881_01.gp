a(n) = sum(j=1, n, sum(k=1, n, gcd(j*k, n)));
for(n=1, 48, print1(a(n),", "))