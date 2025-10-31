\\ G.f.: c(x)/( sqrt(1-4*x) * (1-x*c(x)^5) ), where c(x) is the g.f. of A000108.
my(N=30, x='x+O('x^N), c=2/(1+sqrt(1-4*x))); Vec( c )
my(N=30, x='x+O('x^N), c=2/(1+sqrt(1-4*x))); Vec( c/( sqrt(1-4*x) * (1-x*c^5) ) )