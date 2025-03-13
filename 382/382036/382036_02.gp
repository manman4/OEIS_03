\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-x * C(x)^2) ), where C(x) = 1 + x*C(x)^2 is the g.f. of A000108.
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse(x*exp(-x * sum(k=0, N, binomial(2*k,k)/(k+1)*x^k)^2 ))/x ))



