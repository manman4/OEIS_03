M=39;

a(n) = -sumdiv(n, d, (-n/d)^d*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));