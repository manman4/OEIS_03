M=17;

a136630(n, k) = 1/(2^k*k!)*sum(j=0, k, (-1)^(k-j)*(2*j-k)^n*binomial(k, j));
\\ a(n) = (1/(n+1)) * Sum_{k=0..n} k! * binomial(3*n+3,k) * A136630(n,k).
a(n) = sum(k=0, n, k! * binomial(3*n+3,k) * a136630(n, k))/(n+1);
for(n=0, M, print1(a(n),", "));

