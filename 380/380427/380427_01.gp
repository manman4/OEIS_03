\\ E.g.f. A(x) satisfies A(x) = exp( -x/A(x) * exp(x/A(x)) ).
my(A=1, n=25); for(i=1, n, A = exp( -x/A * exp(x/A + x*O(x^n)) ) ); Vec(serlaplace(A))

\\ E.g.f.: 1/B(x), where B(x) is the e.g.f. of A162695.
my(A=1, n=25); for(i=1, n, A = exp( -x/A * exp(x/A + x*O(x^n)) ) ); Vec(serlaplace(1/A))
