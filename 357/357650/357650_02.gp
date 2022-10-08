M=20;

a(n) = sum(k=0, n\2, 4^(n-2*k)*stirling(n, 2*k, 2));
for(n=0, M, print1(a(n), ", "));