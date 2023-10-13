M=34;

a(n) = sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1)*(d^5-(d-1)^5)));
for(n=1, M, print1(a(n), ", "));