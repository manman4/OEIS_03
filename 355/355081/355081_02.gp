M=20;

a(n)=local(A=1); for(i=1, n, A=1+x*subst( A, x, 3*x/(1-x+x*O(x^n)) )/(1-x+x*O(x^n))); polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));