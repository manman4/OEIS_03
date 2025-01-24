\\ E.g.f. satisfies A(x) = exp( 3 * x * exp(x) * A(x)^(1/3) ).
my(A=1, n=25); for(i=1, n, A = exp( 3 * x * exp(x) * A^(1/3) ) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A273954.
my(A=1, n=25); for(i=1, n, A = exp( 3 * x * exp(x) * A^(1/3) ) ); Vec(serlaplace(A^(1/3)))