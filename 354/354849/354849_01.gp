M=21;

a(n) = (n-1)!*sumdiv(n, d, d^(n/d)/(d-1)!);
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==(n+(n-1)!), print1(n, ", ")));