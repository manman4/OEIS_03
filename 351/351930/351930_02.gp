M=27;

a(n) = n!*sum(k=0, n\4, (-1/4!)^k*binomial(n-3*k, k)/(n-3*k)!);
for(n=0, M, print1(a(n), ", "));