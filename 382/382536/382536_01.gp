M=27;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * binomial(3*k/2,n-k).
a(n) = sum(k=0, n, 4^(n-k)*binomial(3*k/2,n-k));
for(n=0, M, print1(a(n),", "));

\\ a(62) is negative.
print("------")
print1(a(62));
