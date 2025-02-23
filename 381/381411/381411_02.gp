\\ E.g.f. A(x) satisfies A(x) = exp( sinh(x * A(x)^2) / A(x)^2 ).
my(A=1, n=22); for(i=1, n, A=exp( sinh(x*A^2) / A^2 + x*O(x^n) ) ); Vec(serlaplace(A))
