M=43;

a(n) = sum(k=1, n, sumdiv(k, d, kronecker(-4, k/d)*(d^3-(d-1)^3)));
for(n=1, M, print1(a(n), ", "));