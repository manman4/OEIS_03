\\ G.f. A(x) satisfies A(x) = x/(1 - 3*x) + A(x^3).
a(n) = my(A=x); for(i=1, n, A = x/(1-3*x +x*O(x^n)) + subst(A, x, x^3) ); Vec(A);
a(40)
