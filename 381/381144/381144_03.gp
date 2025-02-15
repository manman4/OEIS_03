\\ E.g.f. A(x) satisfies A(x) = exp( x * A(x) * cos(x * A(x)) ).
my(A=1, n=25); for(i=1, n, A= exp( x * A * cos(x * A + x*O(x^n)) ) ); Vec(serlaplace(A))
