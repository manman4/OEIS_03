M=42;

a(n) = -sumdiv(n, d, (-n/d)^d*binomial(d+n/d-2, d-1));
for(n=1, M, print1(a(n), ", "));