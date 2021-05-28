M=44;

a(n) = sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1)*(d^3-(d-1)^3)));
for(n=1, M, print1(a(n), ", "));