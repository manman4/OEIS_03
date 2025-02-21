M=21;

a136630(n, k) = 1/(2^k*k!)*sum(j=0, k, (-1)^(k-j)*(2*j-k)^n*binomial(k, j));
for(n=0, 10, for(k=0, n, print1(a136630(n, k),", ")); print);

\\ a(n) = Sum_{k=0..n} k! * 2^((n-k)/2) * A136630(n,k).
a(n) = sum(k=0, n, k! * 2^((n-k)/2) * a136630(n, k));
for(n=0, M, print1(a(n), ", "));



