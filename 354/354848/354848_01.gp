M=20;

a(n) = (n-1)!*sumdiv(n, d, d^(n/d+1));
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==((n-1)!+n*n!), print1(n, ", ")));