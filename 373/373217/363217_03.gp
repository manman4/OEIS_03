a(n) = my(A=x); for(i=1, n, A = x/(1-x +x*O(x^n)) + subst(A, x, x^7) ); Vec(A);
a(100)
