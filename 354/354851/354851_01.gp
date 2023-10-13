M=21;

a(n) = (n-1)!*sumdiv(n, d, d^(n/d));
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==((n-1)!+n!), print1(n, ", ")));