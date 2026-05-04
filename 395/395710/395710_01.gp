\\ a(n) = (n+1) * Sum_{k=0..n} 4^(n-k) * binomial(5*n+5,k).
a(n) = (n+1) * sum(k=0, n, 4^(n-k) * binomial(5*n+5,k));
for(n=0, 25, print1(a(n), ", "));

\\ a(n) = (n+1) * Sum_{k=0..n} 5^(n-k) * binomial(4*n+k+4,k).
b(n) = (n+1) * sum(k=0, n, 5^(n-k) * binomial(4*n+k+4,k));
for(n=0, 25, print1(a(n)-b(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/4) * log( Sum_{k>=0} binomial(5*k-1,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/4)*log(sum(k=0, N, binomial(5*k-1,k)*x^k)))

