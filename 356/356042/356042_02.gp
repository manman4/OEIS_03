M=44;

a(n) = sum(k=1, n, sumdiv(k, d, d^2*numdiv(k/d)));
for(n=1, M, print1(a(n), ", "));