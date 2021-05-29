M=20;

a(n) = sum(k=1, n, n\k*(-n)^(k-1));
for(n=1, M, print1(a(n), ", "));