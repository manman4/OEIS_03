M=15;

a(n) = my(A=1+x); for(i=1, n, A = (1+x)^(A^2+x*O(x^n)) ); n!*polcoef(A, n);
for(n=0, M, print1(a(n), ", "));