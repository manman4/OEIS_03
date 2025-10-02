\\ a(n) = [x^n] 1/(1 - x^2 / (1 - x)^5)^n.
a(n) = my(x='x+O('x^(n+1))); polcoef( 1/(1 - x^2 / (1 - x)^5)^n, n);
for(n=0, 27, print1(a(n), ", "));

\\ See A389347.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1 - x^2 / (1 - x)^5) )/x)
