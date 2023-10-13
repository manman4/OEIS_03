M=17;

a(n) = (2*n)!*sum(k=n, 2*n, (2*n+1-k)*abs(stirling(k, n, 1))/k!);
for(n=0, M, print1(a(n), ", "));