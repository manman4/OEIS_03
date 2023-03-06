a(n) = local(A=1); for(i=1, n, A=exp(x*A^2)/(1-x+x*O(x^n) )); n!*polcoeff(A, n); 
for(n=0, 17, print1(a(n),", "))
