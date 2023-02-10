M=17;

a(n) = local(A=x); for(i=0, n, A=x*exp(2*A+x*exp(2*A)+O(x^n))); n!*polcoeff(A, n);  
for(n=0, M, print1(a(n), ", "));