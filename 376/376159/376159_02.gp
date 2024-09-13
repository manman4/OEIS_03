\\ a(n) = Sum_{k=0..n} binomial(n+5*k+2,n-k) * binomial(2*k,k)/(k+1).
a(n) = sum(k=0, n, binomial(n+5*k+2,n-k) * binomial(2*k,k)/(k+1));
for(n=0, 22, print1(a(n), ", "))

