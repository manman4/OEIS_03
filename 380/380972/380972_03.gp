\\ E.g.f. A(x) satisfies A(x) = exp( x*A(x) * exp(2*x*A(x)) ).
my(A=1, n=22); for(i=1, n, A=exp( x*A * exp(2*x*A) + x*O(x^n)) ); Vec(serlaplace(A))

