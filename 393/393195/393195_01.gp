\\ G.f. A(x) satisfies A(x) = A(x^2) + x*A(x^2)^3.
a(n) = my(A=1); for(i=0, #binary(n), A=subst(A, x, x^2+x*O(x^n)) + x*subst(A, x, x^2+x*O(x^n))^3); Vec(A);
a(50)