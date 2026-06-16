\\ Let R(x) = Series_Reversion( (1-(1-k*(k+1)*x) * exp(-(k+1)*x))/(k+1)^2 ).
\\ A(x) = R(x) * exp(-k*R(x)).
my(k=4, N=20, x='x+O('x^N), R=serreverse((1-(1-k*(k+1)*x)*exp(-(k+1)*x))/(k+1)^2)); Vec(serlaplace(R*exp(-k*R)))
