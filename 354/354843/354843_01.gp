M=21;

a(n) = n!*sumdiv(n, d, (n/d)^d/d!);
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==(1+n*n!), print1(n, ", ")));