a(n) = sum(k=0, n\4, n^k*binomial(n-3*k, k));
for(n=0, 27, print1(a(n), ", "))