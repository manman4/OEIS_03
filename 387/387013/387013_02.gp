\\ E.g.f. A(x) satisfies A(x) = exp(x * (1+x^2)^2 * A(x)).
my(A=1, n=30); for(i=1, n, A=exp( x * (1+x^2)^2 * A ) + x*O(x^n)); Vec(serlaplace(A))
