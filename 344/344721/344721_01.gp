M=44;

a(n) = sum(k=1, n, (-1)^(k+1)*(n\k)^3);
for(n=1, M, print1(a(n), ", "));