M=90;

a(n) = sumdiv(5*n-2, d, d%5==4);
for(n=1, M, print1(a(n), ", "));