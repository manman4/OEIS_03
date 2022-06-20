M=20;

a(n)=local(A=1); for(i=1, n, A=1-log(1-x+x*O(x^n))*subst( A, x, -2*log(1-x+x*O(x^n)) )); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));