a(n) = local(A=1); for(i=1, n, A=exp( 2*x*A/(1-x+x*O(x^n)) )); n!*polcoeff(A, n); 
for(n=0, 17, print1(a(n),", "))
