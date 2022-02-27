M=27;

a(n) = n!*sum(k=0, n\3, (-1/3!)^k*binomial(n-2*k, k)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));