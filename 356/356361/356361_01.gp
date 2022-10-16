M=21;

a(n) = sum(k=0, n\3, n^k*abs(stirling(n, 3*k, 1)));
for(n=0, M, print1(a(n), ", "));