\\ E.g.f. A(x) satisfies A(x) = exp( sin(x) * A(x)^2 ).
my(A=1, n=25); for(i=1, n, A= exp( sin(x + x*O(x^n)) * A^2 ) ); Vec(serlaplace(A))


