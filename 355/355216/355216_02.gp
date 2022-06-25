M=16;

a(n)=local(A, F); if(n<0, 0, A=O(x); F=exp(x+x*O(x^n))-1; for(m=1, n, A=intformal( 1+subst( A, x, 2*F+x*O(x^m) )/2)); n!*polcoef(A, n));
for(n=1, M, print1(a(n), ", "));