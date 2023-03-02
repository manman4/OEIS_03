M=18;

a(n) = local(A=1); for(i=1, n, A=exp( x/(1-x/A^2+x*O(x^n)) )); n!*polcoeff(A, n); 
for(n=0, M, print1(a(n),", "))
