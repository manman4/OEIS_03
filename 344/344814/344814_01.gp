M=28;

a(n) = sum(k=1, n, n\k*3^(k-1));
for(n=1, M, print1(a(n), ", "));