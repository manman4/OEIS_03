M=44;

a(n) = sum(k=0, n\8, (-1)^k*binomial(n-4*k, 4*k));
for(n=0, M, print1(a(n), ", "));