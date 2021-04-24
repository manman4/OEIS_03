M=40;

a(n) = sumdiv(n, d, 2^(d-n/d)* binomial(d, n/d)); 
for(n=1, M, print1(a(n), ", "));