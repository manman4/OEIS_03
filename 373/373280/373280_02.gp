\\ G.f. A(x) satisfies A(x) = x/(1 - 4*x) + A(x^4).
a(n) = my(A=x); for(i=1, n, A = x/(1-4*x +x*O(x^n)) + subst(A, x, x^4) ); Vec(A);
a(40)
