\\ E.g.f.: sqrt( (1/x) * Series_Reversion( x * (exp(-x) - x)^2 ) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( sqrt( serreverse(x*(exp(-x)-x)^2)/x )))

