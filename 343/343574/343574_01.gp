M=18;

a(n) = sumdiv(n, d, d^d*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));