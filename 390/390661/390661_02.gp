\\ G.f. A(x) satisfies A(x) = 1 / ((1 - x) * (1 - 2 * x * A(x^4))).
seq(n) = my(A=1); for(i=1, n, A=1 / ((1 - x) * (1 - 2 * x * subst(A, x, x^4) +x*O(x^n))) ); Vec(A); 
seq(30)        