M=5;
a(n) = polcoef(prod(k=0, n^n\2, 1+x^(2*k+1)+x*O(x^(n^n))), n^n);
for(n=1, M, print1(a(n), ", "));