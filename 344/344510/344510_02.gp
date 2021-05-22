M=52;

a(n) = n*sumdiv(n, d, eulerphi(n/d)*(d+1))/2;
for(n=1, M, print1(a(n), ", "));