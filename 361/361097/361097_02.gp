M=17;

a(n) = local(A=1); for(i=1, n, A=exp( 1/(1-x/A^3+x*O(x^n)) -1 )); n!*polcoeff(A, n); 
for(n=0, M, print1(a(n),", "));
