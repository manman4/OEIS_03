\\ E.g.f. satisfies A(x) = 1/(1 - x*A(x))^3.
my(A=1, n=22); for(i=1, n, A=1/(1 - x * A +x*O(x^n) )^3 ); Vec(serlaplace(A))



