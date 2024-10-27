\\ E.g.f. A(x) satisfies A(x) = ( 1 - x*A(x)*log(1 - x*A(x)) )^2.
my(A=1, n=32); for(i=1, n, A=(1 - x*A*log(1 - x*A))^2 +x*O(x^n) ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=(1 - x*A*log(1 - x*A))^2 +x*O(x^n) ); Vec(serlaplace(A^(1/2)))

