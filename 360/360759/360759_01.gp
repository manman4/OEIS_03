M=17;

a(n) = sumdiv(n, d, d^(d+n/d)*binomial(d, n/d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, print1(a(n)-n^(n+2), ", "));