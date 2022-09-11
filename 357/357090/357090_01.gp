M=18;

a(n) = sum(k=0, n\2, (2*k)!*(n+k+1)^(k-1)*abs(stirling(n, 2*k, 1))/k!);
for(n=0, M, print1(a(n), ", "));