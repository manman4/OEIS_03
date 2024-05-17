a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, gcd([i, j, k, n])^5)));
for(n=1, 50, print1(a(n), ", "))

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, gcd([i, j, k, l, m, n])^3)))));
for(n=1, 20, print1(a(n)-b(n), ", "))
