\\ a(n) = [x^n] ( (1+3*x)^2/(1-2*x) )^n.
a(n) = polcoef( ( (1+3*x)^2/(1-2*x +x*O(x^n)))^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A386769.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  

