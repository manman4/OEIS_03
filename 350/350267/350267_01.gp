\\ E.g.f.: (exp(x/(1-x)) - exp(x))/x.
my(N=20, x='x+O('x^N)); concat(0, Vec(serlaplace( (exp(x/(1-x)) - exp(x))/x ) ))

my(N=20, x='x+O('x^N)); serlaplace( (exp(x/(1-x)) - exp(x))/x ) 
