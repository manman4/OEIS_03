M=17;

a(n) = sum(k=0, n, (n+k+1)^(k-1)*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));