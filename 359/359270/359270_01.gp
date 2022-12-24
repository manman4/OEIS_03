M=90;

a(n) = sumdiv(5*n-3, d, d%5==3);
for(n=1, M, print1(a(n), ", "));