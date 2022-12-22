M=90;

a(n) = sumdiv(3*n-1, d, d%3==1);
for(n=1, M, print1(a(n), ", "));