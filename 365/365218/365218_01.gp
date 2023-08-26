M=10000;

a(n) = (1/(6*n+1)) * sum(k=0, n, (-1)^(n-k) * binomial(6*n+1,k) * binomial(n-1,n-k) );
for(n=0, M, write("/Users/xxx/Desktop/b365218_10000.txt", n, " ", a(n)))
