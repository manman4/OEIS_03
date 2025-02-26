M=19;

a136630(n, k) = 1/(2^k*k!)*sum(j=0, k, (-1)^(k-j)*(2*j-k)^n*binomial(k, j));
\\ a(n) = (1/(2*n+1)) * Sum_{k=0..n} k! * binomial(2*n+1,k) * i^(n-k) * A136630(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, k! * binomial(2*n+1,k) * I^(n-k) * a136630(n, k))/(2*n+1);
for(n=0, M, print1(a(n),", "));

