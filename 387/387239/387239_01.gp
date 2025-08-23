\\ a(n) = Sum_{k=0..n} binomial(n+3,k+3) * binomial(2*k+6,k+6).
a(n) = sum(k=0, n, binomial(n+3,k+3) * binomial(2*k+6,k+6));
for(n=0, 25, print1(a(n),", "))
