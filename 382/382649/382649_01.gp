M=26;

\\ a(n) = Sum_{k=0..n} 4^(n-k) * (k+1) * binomial(3*k/2,n-k).
a(n) = sum(k=0, n, 4^(n-k)*(k+1)*binomial(3*k/2,n-k));
for(n=0, M, print1(a(n),", "));

print("a(82) is negative.");
print(a(82));




