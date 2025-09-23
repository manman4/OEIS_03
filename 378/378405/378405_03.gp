\\ a(n) = Sum_{k=0..n} binomial(n,k) * A387288(k). 
a387288(n) = sum(k=0, n\2, binomial(n, k)*binomial(2*k, n-2*k));
a(n) = sum(k=0, n, binomial(n,k) * a387288(k));
for(n=0, 25, print1(a(n), ", "))

