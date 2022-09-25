M=17;

a(n) = sum(k=1, n, (2*k)^(k-1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));