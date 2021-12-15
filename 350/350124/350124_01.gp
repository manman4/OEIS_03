M=40;

a(n) = sum(k=1, n, k^2*(n\k)^3);
for(n=1, M, print1(a(n), ", "));