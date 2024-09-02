\\ E.g.f.: A(x) = ( (1/x) * Series_Reversion(x / (2 - exp(x))) )^3.
my(A=1, n=22); for(i=1, n, A=(2 - exp(x*A^(1/3)) +x*O(x^n))^3  ); Vec(serlaplace(A))

my(A=1, n=22); for(i=1, n, A=(2 - exp(x*A^(1/3)) +x*O(x^n))^3  ); Vec(serlaplace(A^(1/3)))


