\\ a(n) = [x^n] ((1+x)^2 * (x+3*(1+x)^2))^n.
a(n) = polcoef( ( (1+x)^2 * (x+3*(1+x)^2) + x*O(x^n) )^n, n);
for(n=0, 22, print1(a(n), ", "));

\\ See A388916.
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 

my(N=30, x='x+O('x^N)); Vec(serreverse( x / ((1+x)^2 * (x+3*(1+x)^2)) )/(3*x))
