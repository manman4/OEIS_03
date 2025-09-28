\\ G.f.: 1 + Series_Reversion( x / (x+(1+x)^5) ).
my(N=40, x='x+O('x^N)); Vec(1 + serreverse( x / (x+(1+x)^5) ))  
