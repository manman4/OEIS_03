M=22;

a(n) = n!*sumdiv(n, d, 1/d!^(n/d-1));
for(n=1, M, print1(a(n), ", "));