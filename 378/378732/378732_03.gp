\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+1,k) * binomial(4*k,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1, k) * binomial(4*k, n-k));

\\ G.f.: A(x) = (1 + x*B(x))^4 where B(x) is the g.f. of A364743.
my(N=30, x='x+O('x^N)); Vec( ( 1 + x * sum(k=0, N, a(k) * x^k) )^4 )    
