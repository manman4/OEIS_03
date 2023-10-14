M=18;

a(n) = n!*sumdiv(n, d, d^d/d!);
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==(n^n+n!), print1(n, ", ")));