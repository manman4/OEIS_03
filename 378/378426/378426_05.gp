\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389062(k).
a389062(n) = sum(k=0, n\2, binomial(n+1,k) * binomial(2*k,n-2*k))/(n+1);
a(n) = sum(k=0, n, binomial(n,k) * a389062(k));
for(n=0, 25, print1(a(n), ", "))

