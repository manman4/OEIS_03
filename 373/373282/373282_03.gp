\\ G.f. A(x) satisfies A(x) = x/(1 - 6*x) + A(x^6).
a(n) = my(A=x); for(i=1, n, A = x/(1-6*x +x*O(x^n)) + subst(A, x, x^6) ); Vec(A);
a(40)
