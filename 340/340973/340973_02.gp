a(n) = sum(k=0, n, 3^k*binomial(n, k)*binomial(2*k, k));
for(n=0, 20, print1(a(n), ", "))