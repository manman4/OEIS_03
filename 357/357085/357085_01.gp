M=19;

a(n) = sum(k=0, n\3, (3*k)!*(n+k+1)^(k-1)*stirling(n, 3*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));