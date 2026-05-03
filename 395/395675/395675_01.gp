\\ a(n) = ((n+1)/2) * (binomial(2*n+1,n) + 2^(2*n)).
a(n) = ((n+1)/2) * (binomial(2*n+1,n) + 2^(2*n));
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = ((n+1)/4) * (binomial(2*n+1,n) + Sum_{k=0..n+1} binomial(2*n+2,k))
b(n) = ((n+1)/4) * (binomial(2*n+1,n) + sum(k=0, n+1, binomial(2*n+2,k)));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = A339240(n+1)/2.
a339240(n) = n*2^(2*n-2) + n*binomial(2*n, n)/2;
for(n=0, 20, print1(a(n)-a339240(n+1)/2, ", "));

\\ Sum_{k>=1} a(k-1) * x^k/k^2 = (1/4) * log( Sum_{k>=0} binomial(2*k+2,k) * x^k ).
my(N=30, x='x+O('x^N)); Vec(sum(k=1, N, a(k-1)*x^k/k^2) - (1/4)*log(sum(k=0, N, binomial(2*k+2,k)*x^k)))