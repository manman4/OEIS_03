M=24;

a(n)=local(A, F); if(n<0, 0, A=O(x); F=exp(x+x*O(x^n))-1; for(m=1, n, A=intformal( 1+2*F*A)); n!*polcoef(A, n));
for(n=0, M, print1(a(n), ", "));