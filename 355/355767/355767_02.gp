M=19;

a(n) = local(A=1); for(i=1, n, A=1/(1-x*A+x*O(x^n))^(1/A^2) ); n!*polcoef(A, n);
for(n=0, M, print1(a(n), ", "));