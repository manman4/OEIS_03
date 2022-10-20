M=33;

a(n) = sum(k=0, n\3, stirling(n-2*k, k, 2));
for(n=0, M, print1(a(n), ", "));