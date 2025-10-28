\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * binomial(n-k-1,k) * A001764(n-2*k).
a001764(n) = binomial(3*n, n)/(2*n+1);
a(n) = sum(k=0, n\2, (-1)^k * binomial(n-k-1, k) * a001764(n-2*k) );
for(n=0, 25, print1(a(n),", "));

b(n) = sum(k=0, n\2, (-1)^k*binomial(n-k-1, k)*binomial(3*(n-2*k), n-2*k)/(2*(n-2*k)+1));
for(n=0, 100, print1(a(n)-b(n),", "));
