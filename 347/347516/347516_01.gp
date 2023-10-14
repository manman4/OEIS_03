M=105;

a(n) = sumdiv(n, d, d^3<=n);
for(n=1, M, print1(a(n), ", "));