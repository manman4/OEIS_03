a(n) = sum(k=0, n\2, n^(3*(n-2*k))*binomial(n-k, k));
for(n=0, 12, print1(a(n), ", "))