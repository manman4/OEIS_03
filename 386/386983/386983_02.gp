\\ E.g.f. A(x) satisfies A(x) = exp(x^2 * (1+x)^2 * A(x)).
my(A=1, n=50); for(i=1, n, A=exp( x^2 * (1+x)^2 * A ) + x*O(x^n)); Vec(serlaplace(A))
