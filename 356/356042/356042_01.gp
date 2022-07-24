M=44;

a(n) = sum(k=1, n, sigma(k, 2)*(n\k));
for(n=1, M, print1(a(n), ", "));