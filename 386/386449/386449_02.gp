\\ G.f. A(x) satisfies A(x) = 1/(1 - x - x^4*A'''(x)).

my(A=1, n=20); for(i=1, n, A= 1/(1 - x - x^4*deriv(deriv(deriv(A)))) + x*O(x^n) ); Vec(A)

