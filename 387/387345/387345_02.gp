\\ Coefficient of x^n in the expansion of ( (1-x+x^4) / (1-x) )^n.
a(n) = my(x='x+O('x^(n+1))); polcoef( ((1-x+x^4) / (1-x))^n, n);
for(n=0, 27, print1(a(n), ", "));

\\ See A215341.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1-x) / (1-x+x^4) )/x)