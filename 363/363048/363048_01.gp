T(n, k) = polcoef(sum(i=0, n, x^(k*i)/prod(j=1, k*i, 1-x^j+x*O(x^n))), n);

M=12;
for(n=0, M, for(k=0, n, print1(T(n,k),", ")))