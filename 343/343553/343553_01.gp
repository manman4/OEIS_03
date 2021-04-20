M=27;

a(n) = sumdiv(n, d, eulerphi(n/d)*binomial(d+n-2, n-1));
for(n=1, M, print1(a(n), ", "));