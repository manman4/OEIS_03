M=3;

a(n) = polcoef(prod(k=0, 10^n\2, 1+x^(2*k+1)+x*O(x^(10^n))), 10^n);
for(n=0, M, print1(a(n), ", "));