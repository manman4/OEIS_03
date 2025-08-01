\\ a(n) = [x^n] ( (1+x)^3/(1-2*x)^2 )^n.
a(n) = polcoef( ( (1+x)^3/(1-2*x +x*O(x^n))^2 )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A385474.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  

