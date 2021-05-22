M=18;

a(n) = sum(k=1, n, eulerphi(k)*(n\k)^n);
for(n=1, M, print1(a(n), ", "));