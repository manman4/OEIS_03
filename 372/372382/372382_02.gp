a(n) = polcoef( ( (1+x+x^2)^4 / (1+x + x*O(x^n))^3 )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A372383.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  


