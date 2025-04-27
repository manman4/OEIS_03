\\ E.g.f.: -5 - 2*x + (-12*x + 5*exp(x))/(1-x)^2.
my(N=30, x='x+O('x^N)); Vec(serlaplace( -5 - 2*x + (-12*x + 5*exp(x))/(1-x)^2 ))
