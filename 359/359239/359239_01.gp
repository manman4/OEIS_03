M=90;

a(n) = sumdiv(3*n-2, d, d%3==2);
for(n=1, M, print1(a(n), ", "));