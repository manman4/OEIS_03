M=1000;

\\ a(n) = (1/4) * Sum_{k=0..floor(n/2)} (k+2) * 2^(n-2*k) * binomial(2*k+2,2*n-4*k+1).
a(n) = (1/4) * sum(k=0, n\2, (k+2) * 2^(n-2*k) * binomial(2*k+2, 2*n-4*k+1));
for(n=0, M, write("/Users/xxx/Desktop/b391836_1.txt", n, " ", a(n)));
