M=22;

a(n) = sum(k=0, (n-1)\3, 2^k*abs(stirling(n, 3*k+1, 1)));
for(n=0, M, print1(a(n), ", "));