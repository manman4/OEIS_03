M=33;

a(n) = sumdiv(n, d, 3^(d-n/d)* binomial(d, n/d)); 
for(n=1, M, print1(a(n), ", "));