M=20;

a(n)=local(A=x); for(i=1, n, A=x*(exp(A+x^3+x*O(x^n)))); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", ")); 
