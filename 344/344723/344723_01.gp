M=34;

a(n) = sum(k=1, n, (-1)^(k+1)*(n\k)^5);
for(n=1, M, print1(a(n), ", "));