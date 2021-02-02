a(n) = sum(k=0, n, n^k*binomial(n, k)*binomial(2*k, k));
for(n=0, 20, print1(a(n), ", "))