\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-x * B(x)^3) ), where B(x) = 1 + x*B(x)^3 is the g.f. of A001764.
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse(x*exp(-x * sum(k=0, N, binomial(3*k,k)/(2*k+1)*x^k)^3 ))/x ))



