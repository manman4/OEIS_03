\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * binomial(3*n-8*k-1,k) * Catalan(n-3*k).
catalan(n) = binomial(2*n,n)/(n+1);
a(n) = sum(k=0, n\3, (-1)^k * binomial(3*n-8*k-1, k) * catalan(n-3*k) );
for(n=0, 29, print1(a(n),", "));   

b(n) = sum(k=0, n\3, (-1)^k*binomial(3*n-8*k-1, k)*binomial(2*(n-3*k), n-3*k)/(n-3*k+1));
for(n=0, 100, print1(a(n)-b(n),", "));  
