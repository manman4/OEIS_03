\\ Coefficient of x^n in the expansion of ( (1+x)^2 * (1+x+x^2)^2 )^n.
a(n) = polcoef( ( (1+x)^2 * (1+x+x^2)^2 )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A369478.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  

