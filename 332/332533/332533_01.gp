M=20;

a(n) = sum(k=1, n, sumdiv(k, d, n^(d-1))); 
for(n=1, M, print1(a(n), ", "));