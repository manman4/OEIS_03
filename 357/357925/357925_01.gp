M=34;

a(n) = sum(k=0, n\3, stirling(n-2*k, n-3*k, 2));
for(n=0, M, print1(a(n), ", "));