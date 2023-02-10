M=19;

a(n) = local(A=x); for(i=0, n, A=x*exp(-A+x*exp(-A)+O(x^n))); n!*polcoeff(A, n);  
for(n=0, M, print1(a(n), ", "));