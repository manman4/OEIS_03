M=21;

a(n) = sum(k=0, n\4, (4*k)!*stirling(n, 4*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));