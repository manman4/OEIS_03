\\ E.g.f. A(x) satisfies A(x) = exp( arcsinh(x * A(x)) / A(x) ).
my(A=1, n=20); for(i=1, n, A= exp( asinh(x*A + x*O(x^n)) / A )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = ( x*A(x) + sqrt((x*A(x))^2 + 1) )^(1/A(x)).
my(A=1, n=20); for(i=1, n, A= (x*A + sqrt((x*A)^2 + 1) + x*O(x^n))^(1/A) ); Vec(serlaplace(A))