M=15;

a(n) = n!*sumdiv(n, d, d^n/(n/d)!);
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==(1+n^n*n!), print1(n, ", ")));