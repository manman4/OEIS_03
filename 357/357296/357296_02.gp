M=22;

a(n) = n!*sumdiv(n, d, 1/(d^(n/d-1)*d!));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==1+n!, print1(n, ", ")));
