M=37;

a(n) = sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1)*(d^4-(d-1)^4)));
for(n=1, M, print1(a(n), ", "));