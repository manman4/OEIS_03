M=37;

a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, gcd([i, j, k, l])))));
for(n=1, M, print1(a(n), ", "));