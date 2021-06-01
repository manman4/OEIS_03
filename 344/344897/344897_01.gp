default(parisize, 120000000)

M=79;

a(n) = numdiv(10^n+1);
for(n=0, M, print1(a(n), ", "));