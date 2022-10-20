M=37;

a(n) = sum(k=0, n\4, stirling(n-3*k, n-4*k, 2));
for(n=0, M, print1(a(n), ", "));