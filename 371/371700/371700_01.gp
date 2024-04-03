a(n, r=1, t=6, u=1) = r*sum(k=0, n, binomial(n, k)*binomial(t*n+u*k+r, n)/(t*n+u*k+r));
for(n=0, 17, print1(a(n), ", "))

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(6*n+k+1,n)/(6*n+k+1).
b(n) = sum(k=0, n, binomial(n, k)*binomial(6*n+k+1, n)/(6*n+k+1));
for(n=0, 19, print1(a(n)-b(n),", ")) 

\\ a(n) = (1/n) * Sum_{k=0..n-1} (-1)^k * 2^(n-k) * binomial(n,k) * binomial(7*n-k,n-1-k) for n > 0.
c(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k*2^(n-k)*binomial(n,k)*binomial(7*n-k,n-1-k))/n);
for(n=0, 19, print1(a(n)-c(n), ", "))

\\ a(n) = (1/n) * Sum_{k=1..n} 2^k * binomial(n,k) * binomial(6*n,k-1) for n > 0.
d(n) = if(n==0, 1, sum(k=1, n, 2^k*binomial(n,k)*binomial(6*n,k-1))/n);
for(n=0, 19, print1(a(n)-d(n), ", "))

