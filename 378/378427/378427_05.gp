\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389064(k).
a389064(n) = sum(k=0, n\3, binomial(n+1,k) * binomial(3*k,n-3*k))/(n+1);
a(n) = sum(k=0, n, binomial(n,k) * a389064(k));
for(n=0, 25, print1(a(n), ", "))

