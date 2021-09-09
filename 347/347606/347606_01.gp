default(parisize, 12000000000)

M=8;
a(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^n-n)))), n^n-n);

for(n=0, M, print1(a(n), ", "));