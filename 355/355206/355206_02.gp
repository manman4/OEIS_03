M=20;

a(n)=local(A, F); if(n<0, 0, A=O(x); F=exp(x+x*O(x^n))-1; for(m=1, n, A=intformal( subst( 1+2*A, x, F+x*O(x^m)))); n!*polcoef(A, n));
for(n=1, M, print1(a(n), ", "));