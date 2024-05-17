a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, gcd([i, j, k, n])^3)));
for(n=1, 50, print1(a(n), ", "))


