M=34;

a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, gcd([i, j, k, l, m]))))));
for(n=1, M, print1(a(n), ", "));