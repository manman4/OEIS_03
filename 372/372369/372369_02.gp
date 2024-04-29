a(n) = polcoef( ( (1+x+x^2)^3 / (1+x + x*O(x^n)) )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ (1/x) * Series_Reversion( x * (1+x) / (1+x+x^2)^3 )
my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1+x) / (1+x+x^2)^3 ))
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  


