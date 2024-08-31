\\ E.g.f. satisfies A(x) = 1/(1 - x^2*A(x))^x.
my(A=1, n=22); for(i=1, n, A=1/(1 - x^2*A)^x +x*O(x^n)  ); Vec(serlaplace(A))

