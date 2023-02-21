M=33;

a(n) = sumdiv(n, d, 2^(n-d)*binomial(d+n/d-2, d-1));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==1+2^(n-1), print1(n, ", ")));
