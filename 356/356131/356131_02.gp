M=18;

a(n) = sum(k=1, n, k*(sigma(k, n-1)-(n\k)^n));
for(n=1, M, print1(a(n), ", "));