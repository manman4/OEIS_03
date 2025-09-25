\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389156(k).
a389156(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+1,k) * binomial(4*k,n-3*k));
a(n) = sum(k=0, n, binomial(n, k) * a389156(k));
for(n=0, 28, print1(a(n), ", ")); 