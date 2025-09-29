\\ a(n) = [x^n] 1/(1 - x^4 - x^5)^n.
a(n) = polcoef( 1/( 1 - x^4 - x^5 + x*O(x^n) )^n, n);
for(n=0, 27, print1(a(n), ", "));

\\ See A365731.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1 - x^4 - x^5) )/x)
