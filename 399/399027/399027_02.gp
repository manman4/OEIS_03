M=20;

a136630(n, k) = 1/(2^k*k!)*sum(j=0, k, (-1)^(k-j)*(2*j-k)^n*binomial(k, j));
for(n=0, 10, for(k=0, n, print1(a136630(n, k), ", ")));
\\ a(n) = Sum_{k=0..floor(n/2)} A047974(n-2*k) * A136630(n,n-2*k).
b(n) = if(n<2, 1, b(n-1) + 2*(n-1)*b(n-2));
a(n) = sum(k=0, n\2, b(n-2*k) * a136630(n, n-2*k));
for(n=0, M, print1(a(n), ", "))

