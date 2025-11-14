\\ G.f. A(x) satisfies A(x) = 1 / ((1 - x) * (1 - 2 * x * (1 + x + x^2) * A(x^3))).
seq(n) = my(A=1); for(i=1, n, A=1 / ((1 - x) * (1 - 2 * x * (1 + x + x^2) * subst(A, x, x^3) +x*O(x^n))) ); Vec(A); 
seq(30)        