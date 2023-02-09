M=17;

a(n) = local(A=x); for(i=1, n, A=x*exp(x-2*A+x*O(x^n))); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", ")); 