M=23;

a(n) = sum(k=0, (n-1)\2, 4^k*stirling(n, 2*k+1, 2));
for(n=0, M, print1(a(n), ", "));