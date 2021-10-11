M=44;

a(n) = sum(k=0, n\6, (-1)^k*binomial(n-3*k, 3*k));
for(n=0, M, print1(a(n), ", "));