\\ E.g.f. A(x) satisfies A(x) = exp( x * A(x) * cosh(x * A(x)) ).
my(A=1, n=25); for(i=1, n, A= exp( x * A * cosh(x * A + x*O(x^n)) ) ); Vec(serlaplace(A))
