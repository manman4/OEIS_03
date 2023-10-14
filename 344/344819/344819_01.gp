M=26;

a(n) = sum(k=1, n, n\k*(-4)^(k-1));
for(n=1, M, print1(a(n), ", "));