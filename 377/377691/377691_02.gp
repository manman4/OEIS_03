\\ E.g.f. satisfies A(x) = (1 - x * log(1 - x) * A(x))^3.
my(A=1, n=25); for(i=1, n, A=(1 - x * log(1 - x + x*O(x^n)) * A)^3 ); Vec(serlaplace(A))

my(A=1, n=25); for(i=1, n, A=(1 - x * log(1 - x + x*O(x^n)) * A)^3 ); Vec(serlaplace(A^(1/3)))



