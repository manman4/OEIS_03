\\ a(n) = [x^n] ( (1-x)^2 * (1+x)^3 )^n.
a(n) = polcoef( ( (1-x)^2 * (1+x)^3 +x*O(x^n) )^n, n);
for(n=0, 20, print1(a(n), ", "));

\\ See A370107.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k)))  

