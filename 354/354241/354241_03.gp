M=17;

a(n) = sum(k=0, n, (2*k)!*abs(stirling(n, k, 1))/k!);
for(n=0, M, print1(a(n), ", "));