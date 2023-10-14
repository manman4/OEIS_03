M=19;

a(n) = n!*sumdiv(n, d, d^(d-1)/d!);
for(n=1, M, print1(a(n), ", "));

for(n=1, 100, if(a(n)==(n^(n-1)+n!), print1(n, ", ")));