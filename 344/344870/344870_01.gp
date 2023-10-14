default(parisize, 120000000)

M=50;

a(n) = omega(n^n-1);
for(n=2, M, print1(a(n), ", "));