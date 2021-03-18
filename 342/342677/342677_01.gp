M=20;

a(n) = sumdiv(n, d, (n/d)^(n-d+1));
for(n=1, M, print1(a(n), ", "));