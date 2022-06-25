M=17;

a(n)=local(A, F); if(n<0, 0, A=O(x); F=-log(1-x+x*O(x^n)); for(m=1, n, A=intformal( 1+F*subst( A, x, 2*x+x*O(x^m) ))); n!*polcoef(A, n));
for(n=0, M, print1(a(n), ", "));