M=30;

a(n)=local(A=1); for(i=1, n, A=1+x*subst( A, x, -log(1-x+x*O(x^n)) )); n!*polcoef(A, n); 
for(n=0, M, print1(a(n), ", "));