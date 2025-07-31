\\ a(n) = [x^n] ( (1+2*x)^2/(1-x) )^n.
a(n) = polcoef( ( (1+2*x)^2/(1-x +x*O(x^n)) )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A371391.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  

