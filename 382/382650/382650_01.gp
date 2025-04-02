M=25;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * binomial(k+2,2) * binomial(3*k/2,n-k).
a(n) = sum(k=0, n, 4^(n-k)*binomial(k+2,2)*binomial(3*k/2,n-k));
for(n=0, M, print1(a(n),", "));

print("a(100) is negative.");
print(a(100));




