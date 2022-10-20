M=35;

a(n) = sum(k=0, n\4, stirling(n-3*k, k, 2));
for(n=0, M, print1(a(n), ", "));