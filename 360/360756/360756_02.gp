M=63;

a(n) = sumdiv(n, d, 2^(n/d-1)*binomial(d, n/d-1));
for(n=1, M, print1(a(n), ", "));