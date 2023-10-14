M=17;

a(n) = (-1)^(n-1)*sum(k=0, n, (2*k-1)^(k-1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));