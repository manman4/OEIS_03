a(n) = sum(k=0, n-1, binomial(n-1, k)^4*binomial(n, k));
for(n=1, 17, print1(a(n), ", "))