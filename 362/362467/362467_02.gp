M=18;

a(n) = local(A=1); for(i=1, n, A=exp( exp(x/A^2+x*O(x^n)) -1 )); n!*polcoeff(A, n); 
for(n=0, M, print1(a(n),", "))
