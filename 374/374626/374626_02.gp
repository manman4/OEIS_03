a(n) = my(A=1); for(i=0, n, A = subst(A, x, x^3)/(1-3*x + x*O(x^n)) ); polcoeff(A, n);
for(n=0, 27, print1(a(n), ", "))