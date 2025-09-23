\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389061(k). 
a389061(n) = sum(k=0, n\3, binomial(n, k)*binomial(3*k, n-3*k));
a(n) = sum(k=0, n, binomial(n,k) * a389061(k));
for(n=0, 25, print1(a(n), ", "))

