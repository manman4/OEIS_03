default(parisize, 120000000)

M=64;

a(n) = numdiv(n^n+1);
for(n=0, M, print1(a(n), ", "));