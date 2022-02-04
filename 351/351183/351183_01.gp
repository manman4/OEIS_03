M=13;

a(n) = sum(k=0, n, k^(2*n)*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));