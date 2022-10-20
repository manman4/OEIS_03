M=24;

a(n) = sum(k=0, n\3, stirling(n-2*k, k, 1));
for(n=0, M, print1(a(n), ", "));