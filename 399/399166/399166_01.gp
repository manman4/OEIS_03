\\ G.f. A(x) satisfies A(x) = 1/(1 - x * A(x^4)^2).
a(n) = my(A=1+x*O(x^n)); for(i=0, n, A=1/(1-x*subst(A, x, x^4)^2 + x*O(x^n))); Vec(A);
a(50)

