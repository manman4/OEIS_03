\\ Sum_{k=0..n} a(k) * a(n-k) = A079589(n).
a(n) = my(x='x+O('x^(n+3)), b=sum(k=0, n, binomial(5*k, k)*x^k)); polcoef(b/sqrt(1 + 4*(b-1)/5), n);
for(n=0, 20, print1(a(n), ", "));
for(n=0, 50, print1(sum(k=0, n, a(k) * a(n-k)) - binomial(5*n+1, n),", "));

\\ Expansion of B(x)/sqrt(1 + 4*(B(x)-1)/5), where B(x) is the g.f. of A001449.
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(5*k, k)*x^k)); Vec( b )
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(5*k, k)*x^k)); Vec( b/sqrt(1 + 4*(b-1)/5) )