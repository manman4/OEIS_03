\\ a(n) = Sum_{k=0..floor(n/3)} binomial(n-2*k-1,k) * A001764(n-3*k).
a001764(n) = binomial(3*n, n)/(2*n+1);
a(n) = sum(k=0, n\3, binomial(n-2*k-1, k) * a001764(n-3*k) );
for(n=0, 24, print1(a(n),", "));

b(n) = sum(k=0, n\3, binomial(n-2*k-1, k)*binomial(3*(n-3*k), n-3*k)/(2*(n-3*k)+1));
for(n=0, 100, print1(a(n)-b(n),", "));
