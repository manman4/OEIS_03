\\ a(n) = [x^n] ( (1+x)^2/(1-2*x) )^n.
a(n) = polcoef( ( (1+x)^2/(1-2*x +x*O(x^n)) )^n, n);
for(n=0, 20, print1(a(n), ", "));

my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  
\\ (1/x) * Series_Reversion( x * (1-2*x) / (1+x)^2 )
my(N=20, x='x+O('x^N)); Vec(serreverse(x*(1-2*x)/(1+x)^2)/x)
