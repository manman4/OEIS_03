M=21;

a(n) = n!*sumdiv(n, d, d^(n-d)/d!^(n/d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==1+n!, print1(n, ", ")));
