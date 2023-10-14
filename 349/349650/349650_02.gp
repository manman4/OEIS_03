M=17;

a(n) = (-1)^(n-1)*sum(k=0, n, (2*k-1)^(k-1)*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));