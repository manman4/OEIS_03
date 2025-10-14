\\ E.g.f. A(x) satisfies A(x) = exp( x^3*A(x)^3 * (1+x*A(x))^2 ).
my(A=1, n=50); for(i=1, n, A=exp( x^3 * A^3 * (1 + x*A)^2 ) + x*O(x^n)); Vec(serlaplace(A))

