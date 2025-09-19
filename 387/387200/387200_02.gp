\\ Coefficient of x^n in the expansion of (1+x^4+x^5)^n.
a(n) = polcoef( ( 1+x^4+x^5 + x*O(x^n) )^n, n);
for(n=0, 52, print1(a(n), ", "));

\\ See A365730.
my(N=50, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=50, x='x+O('x^N)); Vec(serreverse( x / (1+x^4+x^5) )/x)
