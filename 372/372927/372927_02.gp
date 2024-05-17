a(n) = sum(j=1, n, sum(k=1, n, gcd([j, k, n])^5));
for(n=1, 100, print1(a(n), ", "))

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, gcd([i, j, k, l, m, n])^2)))));
for(n=1, 20, print1(a(n)-b(n), ", "))
