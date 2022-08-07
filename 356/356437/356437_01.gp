M=15;

a(n) = n!*sum(k=1, n, sigma(k, k)/k);
for(n=1, M, print1(a(n), ", "));