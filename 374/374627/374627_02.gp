a(n) = my(A=1); for(i=0, n, A = subst(A, x, x^3)/(1-x + x*O(x^n))^3 ); polcoeff(A, n);
for(n=0, 46, print1(a(n), ", "))