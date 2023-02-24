M=22;

a(n) = n!*sumdiv(n, d, 1/(d-1)!^(n/d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==n+n!, print1(n, ", ")));
