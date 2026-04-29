\\ a(0) = 1; a(n) = Sum_{k=1..floor((n+1)/2)} (-1)^(k-1) * (2*n-2*k+1)^k * binomial(n-k+1,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, (n+1)\2, (-1)^(k-1)*(2*n-2*k+1)^k*binomial(n-k+1, k)*a(n-k)));
for(n=0, 17, print1(a(n),", "));

\\ 1 = Sum_{n>=0} a(n) * x^n * (1 - (2*n+1)*x)^(n+1).
my(N=15, x='x+O('x^N)); sum(n=0, N, a(n) * x^n * (1 - (2*n+1)*x)^(n+1))