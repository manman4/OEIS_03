M=17;

a(n) = sumdiv(n, d, (2*d)^(d-1));
for(n=1, M, print1(a(n), ", "));