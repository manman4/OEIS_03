M=47;

a(n) = sumdiv(n, d, (-1)^(d-1)*d^(n/d-1));
for(n=1, M, print1(a(n), ", "));