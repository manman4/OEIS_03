M=59;

a(n) = sumdiv(n, d, (-1)^(d-1)*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));