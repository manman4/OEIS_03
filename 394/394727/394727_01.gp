\\ Expansion of e.g.f. exp( sqrt(1-x^2) - 1 + x*(1 + arcsin(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(sqrt(1-x^2)-1+x*(1+asin(x)))))

\\ E.g.f. A(x) satisfies A'(x)/A(x) = 1 + arcsin(x).
my(N=30, x='x+O('x^N), A=exp(sqrt(1-x^2)-1+x*(1+asin(x)))); Vec(serlaplace(deriv(A)/A - (1 + asin(x))))