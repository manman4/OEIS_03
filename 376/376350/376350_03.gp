\\ E.g.f. satisfies A(x) = 1/(1 - x^2*A(x)^2)^(x*A(x)).
my(A=1, n=19); for(i=1, n, A=1/(1 - x^2*A^2 + x*O(x^n))^(x*A)); Vec(serlaplace(A))


