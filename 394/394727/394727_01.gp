\\ Expansion of e.g.f. exp( sqrt(1-x^2) - 1 + x*(1 + arcsin(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(sqrt(1-x^2)-1+x*(1+asin(x)))))