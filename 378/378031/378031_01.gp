\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*n,6*k).
a(n) = sum(k=0, n\2, binomial(3*n, 6*k));
for(n=0, 20, print1(a(n),", "));
