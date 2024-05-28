a(n) = my(A=x); for(i=1, n, A = 1/(1-x +x*O(x^n))^2 - 1 + subst(A, x, x^4) ); Vec(A);
a(100)