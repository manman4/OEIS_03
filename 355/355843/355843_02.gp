M=20;

a(n) = local(A=1); for(i=1, n, A=exp( x*(exp(x+x*O(x^n))-1)*A ) ); n!*polcoef(A, n);
for(n=0, M, print1(a(n), ", "));