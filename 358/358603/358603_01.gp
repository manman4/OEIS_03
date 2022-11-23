M=32;

a(n) = sum(k=0, n\2, (-1)^k*(n-k)!/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));