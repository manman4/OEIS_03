M=16;

a(n) = (-1)^n*n!*sum(k=0, n\2, n^(n-k) * stirling(n-k,k,1)/(n-k)!);
for(n=0, M, print1(a(n),", "))
