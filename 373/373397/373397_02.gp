a(n) = my(A=x); for(i=1, n, A = x/(1-x +x*O(x^n))^2 + subst(A, x, x^6) ); Vec(A);
a(100)