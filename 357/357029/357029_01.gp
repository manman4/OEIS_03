M=19;

a(n) = sum(k=0, n\3, (3*k)!*(n+1)^(k-1)*abs(stirling(n, 3*k, 1))/k!);
for(n=0, M, print1(a(n), ", "));