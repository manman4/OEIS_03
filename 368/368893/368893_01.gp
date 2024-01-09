a(n) = sum(k=0, n\3, n^(n-2*k)*binomial(n-2*k, k));
for(n=0, 18, print1(a(n), ", "))