\\ E.g.f. A(x) satisfies A(x) = exp( x^2*A(x)^2 * (1+x*A(x))^3 ) * (1+x*A(x)).
my(A=1, n=50); for(i=1, n, A=exp( x^2 * A^2 * (1 + x*A)^3 ) * (1 + x*A) + x*O(x^n)); Vec(serlaplace(A))

