\\ E.g.f. A(x) satisfies A(x) = exp( x^2*A(x)^2 * (1+x*A(x))^3 ).
my(A=1, n=50); for(i=1, n, A=exp( x^2 * A^2 * (1 + x*A)^3 ) + x*O(x^n)); Vec(serlaplace(A))

