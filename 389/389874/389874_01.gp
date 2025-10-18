\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*(n-3*k),k) * Catalan(n-3*k).
calatan(n) = binomial(2*n,n)/(n+1);
a(n) = sum(k=0, n\3, binomial(3*(n-3*k), k) * calatan(n-3*k) );
for(n=0, 27, print1(a(n),", "))   

b(n) = sum(k=0, n\3, binomial(3*(n-3*k), k)*binomial(2*(n-3*k), n-3*k)/(n-3*k+1));
for(n=0, 100, print1(a(n)-b(n),", "))
