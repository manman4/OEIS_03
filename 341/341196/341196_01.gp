a(n) = sum(k=0, n, k^4*(n-k)!*binomial(n, k)^2);
for(n=0, 20, print1(a(n), ", "))