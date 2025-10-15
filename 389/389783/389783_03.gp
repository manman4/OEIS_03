\\ E.g.f. A(x) satisfies A(x) = exp( x * (1+x)^3 * A(x) ) / (1+x).
my(A=1, n=50); for(i=1, n, A=exp( x * (1+x)^3 * A ) / (1+x) + x*O(x^n)); Vec(serlaplace(A))
