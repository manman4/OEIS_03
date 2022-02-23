M=22;

a(n) = local(A=1+x+x*O(x^n)); for(i=0, n, A=1+x+x^2*subst(A, x, x/(1-4*x))/(1-4*x)); polcoeff(A, n);
for(n=0, M, print1(a(n), ", "));