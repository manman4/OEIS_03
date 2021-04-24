M=33;

a(n) = sumdiv(n, d, 2^(d-1)*binomial(d, n/d));
for(n=1, M, print1(a(n), ", "));