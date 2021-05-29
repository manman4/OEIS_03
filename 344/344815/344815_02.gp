M=26;

a(n) = sum(k=1, n, sumdiv(k, d, 4^(d-1)));
for(n=1, M, print1(a(n), ", "));