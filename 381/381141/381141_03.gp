\\ E.g.f. A(x) satisfies A(x) = exp( x * cos(x) * A(x) ).
my(A=1, n=25); for(i=1, n, A= exp( x * cos(x + x*O(x^n)) * A ) ); Vec(serlaplace(A))


