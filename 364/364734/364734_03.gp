\\ G.f.: 1 + Series_Reversion( x / (1 + x + x * (1 + x)^5) ).
my(N=30, x='x+O('x^N)); Vec(1 + serreverse( x / (1 + x + x * (1 + x)^5) ))


