M=33;

a(n) = sum(k=0, n, (-1)^k*binomial(4*k, n-k));
for(n=0, M, print1(a(n), ", "));