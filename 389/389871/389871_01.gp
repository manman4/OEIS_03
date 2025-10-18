\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*(n-2*k),k) * Catalan(n-2*k).
catalan(n) = binomial(2*n, n)/(n+1);
a(n) = sum(k=0, n\2, binomial(2*(n-2*k), k) * catalan(n-2*k) );
for(n=0, 27, print1(a(n),", "))   

b(n) = sum(k=0, n\2, binomial(2*(n-2*k), k)*binomial(2*(n-2*k), n-2*k)/(n-2*k+1));
for(n=0, 100, print1(a(n)-b(n),", "))
