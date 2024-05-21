a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, gcd(i, n)/gcd([i, j, k, n]))));
for(n=1, 100, print1(a(n), ", "))
