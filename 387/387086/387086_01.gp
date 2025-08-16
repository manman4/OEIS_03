\\ Sum_{k=0..n} a(k) * a(n-k) = A387085(n).
a(n) = my(x='x+O('x^(n+3)), b=sum(k=0, n, binomial(2*k, k)*x^k)); polcoef(b/sqrt(1 + 2*(b-1)), n);
for(n=0, 27, print1(a(n), ", "));
for(n=0, 50, print1(sum(k=0, n, a(k) * a(n-k)) - sum(k=0, n, (-3)^(n-k)*binomial(2*n+1, k)),", "));

\\ Expansion of B(x)/sqrt(1 + 2*(B(x)-1)), where B(x) is the g.f. of A000984.
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(2*k, k)*x^k)); Vec( b )
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(2*k, k)*x^k)); Vec( b/sqrt(1 + 2*(b-1)) )