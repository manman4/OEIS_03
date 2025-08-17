\\ Sum_{k=0..n} a(k) * a(n-k) = A385497(n).
a(n) = my(x='x+O('x^(n+3)), b=sum(k=0, n, binomial(6*k, k)*x^k)); polcoef(b/sqrt(1 + 2*(b-1)/3), n);
for(n=0, 19, print1(a(n), ", "));
for(n=0, 50, print1(sum(k=0, n, a(k) * a(n-k)) - sum(k=0, n, binomial(6*n+1, k)),", "));

\\ Expansion of B(x)/sqrt(1 + 2*(B(x)-1)/3), where B(x) is the g.f. of A004355.
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(6*k, k)*x^k)); Vec( b )
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(6*k, k)*x^k)); Vec( b/sqrt(1 + 2*(b-1)/3) )