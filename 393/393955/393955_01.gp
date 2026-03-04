\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A192949.
my(N=20, x='x+O('x^N)); Vec(serlaplace(1+serreverse(x*exp(-x*(2+x)))))

\\ Expansion of e.g.f. ( 1 + Series_Reversion( x*exp(-x*(2+x)) ) )^2.
my(N=20, x='x+O('x^N)); Vec(serlaplace((1+serreverse(x*exp(-x*(2+x))))^2))