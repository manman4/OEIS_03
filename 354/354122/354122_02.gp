M=19;

a(n) = sum(k=0, n, (k+2)!*abs(stirling(n, k, 1)))/2;
for(n=0, M, print1(a(n), ", "));