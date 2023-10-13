M=25;

a(n) = local(A=1+x*O(x^n)); for(i=0, n, A=1+x*subst(A, x, x/(1-x))/(1-x^2)); polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));