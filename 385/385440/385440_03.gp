\\ E.g.f.: (1/x) * Series_Reversion( x*sqrt(1 - 2*x) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/x * serreverse(x*sqrt(1-2*x))))

\\ E.g.f. A(x) satisfies A(x) = 1/sqrt(1 - 2*x*A(x)). 
my(A=1, n=20); for(i=1, n, A=1/sqrt(1 - 2*x*A + x*O(x^n))); Vec(serlaplace(A))