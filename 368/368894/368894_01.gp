a(n) = sum(k=0, n\2, (-n)^k*binomial(n-k, k));
for(n=0, 24, print1(a(n), ", "))