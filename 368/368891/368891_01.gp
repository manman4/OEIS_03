a(n) = sum(k=0, n\3, n^k*binomial(n-2*k, k));
for(n=0, 27, print1(a(n), ", "))