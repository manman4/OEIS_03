a(n) = sum(k=0, n, (-1)^(n-k)*binomial(n, k)*binomial(4*k, k)); 
for(n=0, 20, print1(a(n),", "))  

\\ The g.f. exp( Sum_{k>=1} a(k) * x^k/k ) has integer coefficients and equals (1/x) * Series_Reversion( x/((1+x)^4 - x) ). See A317133.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k) * x^k/k)))
my(N=30, x='x+O('x^N)); Vec(serreverse( x / ((1+x)^4 - x) )/x)   