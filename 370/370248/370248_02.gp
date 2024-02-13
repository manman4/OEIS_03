\\ Coefficient of x^n in the expansion of ( 1/(1-x) * (1+x^3)^2 )^n.
a(n) = polcoef( ( 1/(1-x + x*O(x^n)) * (1+x^3)^2 )^n, n);
for(n=0, 22, print1(a(n), ", "));

\\ See A369266.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 