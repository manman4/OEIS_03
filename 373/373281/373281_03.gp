\\ G.f. A(x) satisfies A(x) = x/(1 - 5*x) + A(x^5).
a(n) = my(A=x); for(i=1, n, A = x/(1-5*x +x*O(x^n)) + subst(A, x, x^5) ); Vec(A);
a(40)
