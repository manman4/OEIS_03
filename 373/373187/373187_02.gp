a(n) = my(A=x); for(i=1, n, A = x/(1-x +x*O(x^n))^4 + subst(A, x, x^4) ); Vec(A);
a(100)