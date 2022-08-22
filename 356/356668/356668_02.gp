M=22;

a(n) = if(n==0, 1, n!*sumdiv(n, d, 1/(d*(d-1)!^(n/d))));
for(n=0, M, print1(a(n), ", "));