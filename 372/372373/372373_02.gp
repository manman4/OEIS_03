a(n) = polcoef( ( (1+x+x^3)^3 / (1+x + x*O(x^n))^2 )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A372377.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  


