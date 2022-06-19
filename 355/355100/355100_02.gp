M=17;

a(n)=local(A=1); for(i=1, n, A=1+2*x*subst( A, x, exp(x+x*O(x^n))-1 )); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));