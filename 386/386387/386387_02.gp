\\ G.f.: 2/(1 - x + sqrt((1-x) * (1-17*x))).
my(N=20, x='x+O('x^N)); Vec(2/(1-x+sqrt((1-x)*(1-17*x))))

\\ G.f. A(x) satisfies A(x) = 1/(1 - x) + 4*x*A(x)^2.
my(N=20, x='x+O('x^N), A=2/(1-x+sqrt((1-x)*(1-17*x)))); Vec(A - 1/(1-x) - 4*x*A^2)