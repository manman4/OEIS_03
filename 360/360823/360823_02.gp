M=40;

a(n) = sumdiv(n, d, d^(n/d)*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==2*n, print1(n, ", ")));
