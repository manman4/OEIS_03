M=55;

a(n) = sum(k=1, n, sumdiv(k, d, kronecker(-4, k/d)*d));
for(n=1, M, print1(a(n), ", "));