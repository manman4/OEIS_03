M=15;

\\ a(n) = [x^n] 1/((1+x^2) * (1-x)^(2*n-2)).
a(n) = polcoef(1/((1+x^2) * (1-x)^(2*n-2) + x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));