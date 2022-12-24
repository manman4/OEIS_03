M=90;

a(n) = sumdiv(4*n-2, d, d%4==1);
for(n=1, M, print1(a(n), ", "));