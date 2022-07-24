M=39;

a(n) = sum(k=1, n, sigma(k, 3)*(n\k));
for(n=1, M, print1(a(n), ", "));