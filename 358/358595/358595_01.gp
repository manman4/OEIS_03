M=18;

a(n) = n!*sumdiv(n, d, d^n/d!^(n/d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==n^n+n!, print1(n, ", ")));
