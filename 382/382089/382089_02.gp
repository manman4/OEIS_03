\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-x * B(x)^4) ), where B(x) = 1 + x*B(x)^5 is the g.f. of A002294.
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse(x*exp(-x * sum(k=0, N, binomial(5*k,k)/(4*k+1)*x^k)^4 ))/x ))


