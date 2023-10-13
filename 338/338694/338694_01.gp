M=19;

a(n) = sumdiv(n, d, d^d*binomial(d, n/d));
for(n=1, M, print1(a(n), ", "));