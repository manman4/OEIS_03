\\ G.f. A(x) satisfies A(x) = 1/((1 - x) * (1 - x*A(x)^3 - x^2*A(x)^2*A'(x))).
my(A=1, n=20); for(i=1, n, A= 1/((1 - x) * (1 - x*A^3 - x^2*A^2*deriv(A) + x*O(x^n)) )); Vec(A)

