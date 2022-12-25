M=90;

a(n) = sumdiv(6*n-4, d, d%6==1);
for(n=1, M, print1(a(n), ", "));