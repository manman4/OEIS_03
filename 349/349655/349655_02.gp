M=17;

a(n) = sum(k=0, n, (-3*k+1)^(k-1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));