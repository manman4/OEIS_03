M=90;

a(n) = sumdiv(6*n-2, d, d%6==5);
for(n=1, M, print1(a(n), ", "));