M=30;

a(n)=local(A=1); for(i=1, n, A=1+x*subst( A, x, x/(1-x+x*O(x^n))^5 )/(1-x+x*O(x^n))^5); polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));