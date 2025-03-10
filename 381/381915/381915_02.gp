\\ Expansion of (1/x) * Series_Reversion( x * (1-x)^2 / B(x) ), where B(x) is the g.f. of A002293.

my(N=30, x='x+O('x^N), B=sum(k=0, N, binomial(4*k,k)/(3*k+1) * x^k)); Vec(serreverse( x * (1-x)^2 / B )/x)
 