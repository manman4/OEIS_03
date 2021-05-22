M=48;

a(n) = sum(i=1, n, sum(j=i, n, sum(k=j, n, gcd([i, j, k]))));
for(n=1, M, print1(a(n), ", "));