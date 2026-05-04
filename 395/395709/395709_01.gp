\\ a(n) = (n+1) * Sum_{k=0..n} 3^(n-k) * binomial(4*n+4,k).
a(n) = (n+1) * sum(k=0, n, 3^(n-k) * binomial(4*n+4,k));
for(n=0, 25, print1(a(n), ", "));

\\ a(n) = (n+1) * Sum_{k=0..n} 4^(n-k) * binomial(3*n+k+3,k).
b(n) = (n+1) * sum(k=0, n, 4^(n-k) * binomial(3*n+k+3,k));
for(n=0, 25, print1(a(n)-b(n), ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/3) * log( Sum_{k>=0} binomial(4*k-1,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/3)*log(sum(k=0, N, binomial(4*k-1,k)*x^k)))

