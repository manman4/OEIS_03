a(n) = sum(k=0, n, n^(2*k)*(n-k)!*binomial(n, k)^2);
for(n=0, 20, print1(a(n), ", "))