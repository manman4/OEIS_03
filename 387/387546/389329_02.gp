\\ a(n) = [x^n] (1 + x^3 / (1 - x)^4)^n.
a(n) = my(x='x+O('x^(n+1))); polcoef( (1 + x^3 / (1 - x)^4)^n, n);
for(n=0, 27, print1(a(n), ", "));

\\ See A389251.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=30, x='x+O('x^N)); Vec(serreverse( x / (1 + x^3 / (1 - x)^4) )/x)
