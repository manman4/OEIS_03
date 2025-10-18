\\ E.g.f. A(x) satisfies A(x) = exp( x * A(x) / (1-x)^3 ) / (1-x).
my(A=1, n=30); for(i=1, n, A=exp( x * A / (1-x)^3 ) / (1-x) + x*O(x^n)); Vec(serlaplace(A))
