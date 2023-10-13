M=40;

a(n) = sumdiv(n, d, eulerphi(n/d)*binomial(d+n-1, n));
for(n=1, M, print1(a(n),", "));  