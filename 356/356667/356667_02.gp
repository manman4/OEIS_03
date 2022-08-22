M=21;

a(n) = if(n==0, 1, n!*sumdiv(n, d, 1/(d-1)!^(n/d-1)));
for(n=0, M, print1(a(n), ", "));