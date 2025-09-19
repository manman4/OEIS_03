\\ Coefficient of x^n in the expansion of ((1+x) * (1+x^4))^n.
a(n) = polcoef(((1+x)*(1+x^4))^n, n);
for(n=0, 30, print1(a(n), ", ")); 

\\ a(n) = [x^(4*n)] ((1+x) * (1+x^4))^n.
b(n) = polcoef(((1+x)*(1+x^4))^n, 4*n);
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ See A369444.
my(N=50, x='x+O('x^N)); Vec(exp(sum(k=1, N, a(k)*x^k/k))) 
