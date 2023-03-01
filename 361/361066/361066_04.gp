M=16;

a(n) = local(A=1); for(i=1, n, A=exp(x/(1-x+x*O(x^n)) * A^3)); n!*polcoeff(A, n);   
for(n=0, M, print1(a(n),", "))  

