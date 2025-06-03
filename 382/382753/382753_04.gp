\\ a(n) = Sum_{k=0..n} 2^k * 3^(n-k) * k! * Stirling2(n,k).
a(n) = sum(k=0, n, 2^k * 3^(n-k) * k! * stirling(n, k, 2));
for(n=0, 15, print1(a(n),", "));

\\ a(n) = (2/5) * A201367(n)
for(n=0, 15, print1((5/2) * a(n),", "));

\\ a(n) = (2/5) * Sum_{k=0..n} 5^k * (-3)^(n-k) * k! * Stirling2(n,k) for n > 0.
b(n) = (2/5) * sum(k=0, n, 5^k * (-3)^(n-k) * k! * stirling(n, k, 2));
for(n=0, 50, print1(a(n)-b(n),", "));




