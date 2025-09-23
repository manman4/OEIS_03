\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389063(k).
a389063(n) = sum(k=0, n\2, binomial(n+1,k) * binomial(3*k,n-2*k))/(n+1);
a(n) = sum(k=0, n, binomial(n,k) * a389063(k));
for(n=0, 25, print1(a(n), ", "))

