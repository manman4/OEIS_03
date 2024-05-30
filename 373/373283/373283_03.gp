\\ G.f. A(x) satisfies A(x) = x/(1 - 7*x) + A(x^7).
a(n) = my(A=x); for(i=1, n, A = x/(1-7*x +x*O(x^n)) + subst(A, x, x^7) ); Vec(A);
a(40)
