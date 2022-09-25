M=16;

a(n) = sum(k=1, n, (n+3*k)^(k-1)*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));