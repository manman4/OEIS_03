M=21;

a(n) = sum(k=0, n, (k+3)!*stirling(n, k, 1))/6;
for(n=0, M, print1(a(n), ", "));