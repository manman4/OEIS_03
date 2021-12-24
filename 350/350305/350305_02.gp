M=10;

a(n) = polcoef(prod(k=1, n, 1+x^k+x^(2*k))^n, n^2*(n+1)/2);
for(n=0, M, print1(a(n), ", "));