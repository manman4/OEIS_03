M=17;

a(n) = local(A=1+x); for(i=1, n, A=exp(x*subst(A, x, -x+x*O(x^n))^2)); n!*polcoeff(A, n);
for(n=0, M, print1(a(n),", "))  


