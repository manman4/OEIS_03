M=24;

a136630(n, k) = 1/(2^k*k!)*sum(j=0, k, (-1)^(k-j)*(2*j-k)^n*binomial(k, j));
\\ a(n) = Sum_{k=0..n} k! * binomial(n/2-k/2+1,k)/(n/2-k/2+1) * i^(n-k) * A136630(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, k!*binomial(n/2-k/2+1, k)/(n/2-k/2+1)*I^(n-k)*a136630(n, k));
for(n=0, M, print1(a(n),", "));

