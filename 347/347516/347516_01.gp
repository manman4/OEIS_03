M=105;

a(n) = sumdiv(n, d, d*d*d<=n);
for(n=1, M, print1(a(n), ", "));