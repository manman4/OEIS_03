M=17;

a(n)=local(A, F); if(n<0, 0, A=O(x); F=1-exp(-x+x*O(x^n)); for(m=1, n, A=intformal( subst( 1+A/2, x, 2*F+x*O(x^m)))); n!*polcoef(A, n));
for(n=1, M, print1(a(n), ", "));