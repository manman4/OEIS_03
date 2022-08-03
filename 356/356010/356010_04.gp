M=30;

a(n) = n!*sum(k=1, n, sigma(k)/k); 
for(n=1, M, print1(a(n), ", "));