\\ G.f.: A(x) = B(x/(1-x)), where B(x) = (1/x) * Series_Reversion( x*(1-x)^5 ).
\\ B
my(N=30, x='x+O('x^N)); Vec(serreverse( x*(1-x)^5 )/x)
for(n=0, 30, print1( binomial(6*n+4,n)/(n+1),", "))
\\ B(x/(1-x))
my(N=30, x='x+O('x^N)); Vec( sum(k=0, N, binomial(6*k+4,k)/(k+1) * (x/(1-x))^k ) )
