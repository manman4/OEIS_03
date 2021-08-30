M=70;

a(n) = sum(k=1, n, k^3*(n\k^3));
for(n=1, M, print1(a(n), ", "));