\\ E.g.f. satisfies A(x) = exp( 2 * x * exp(x) * A(x)^(1/2) ).
my(A=1, n=25); for(i=1, n, A = exp( 2 * x * exp(x) * sqrt(A) ) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A273954.
my(A=1, n=25); for(i=1, n, A = exp( 2 * x * exp(x) * sqrt(A) ) ); Vec(serlaplace(A^(1/2)))