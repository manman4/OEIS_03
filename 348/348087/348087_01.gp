M=16;

a(n) = polcoef(1/prod(k=1, n, 1-(2*k-1)*x+x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));