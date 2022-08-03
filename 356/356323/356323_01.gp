M=20;

a(n) = n!*sum(k=1, n, sigma(k, 3)/k);
for(n=1, M, print1(a(n), ", "));