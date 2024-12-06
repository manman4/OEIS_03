\\ a(n) = sum(k=0, n, binomial(n+1, k)*binomial(3*k, n-k))/(n+1);
a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1, k) * binomial(3*k, n-k));

\\ G.f.: A(x) = (1 + x*B(x))^2 where B(x) is the g.f. of A364742.
my(N=30, x='x+O('x^N)); Vec( ( 1 + x * sum(k=0, N, a(k) * x^k) )^2 )


