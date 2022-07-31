M=19;

a(n) = sum(k=1, n, k^2*(sigma(k, n-2)-(n\k)^n));
for(n=1, M, print1(a(n), ", "));