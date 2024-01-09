a(n) = sum(k=0, n\2, n^(3*k)*binomial(n-k, k));
for(n=0, 16, print1(a(n), ", "))