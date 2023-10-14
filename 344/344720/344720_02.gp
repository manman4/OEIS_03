M=55;

a(n) = sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1)*(2*d-1)));
for(n=1, M, print1(a(n), ", "));