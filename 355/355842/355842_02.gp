M=20;

a(n) = local(A=1); for(i=1, n, A=1/(1-x+x*O(x^n))^(x*A) ); n!*polcoef(A, n);
for(n=0, M, print1(a(n), ", "));