\\ a(n) = Sum_{k=0..n} 6^(n-k) * k! * Stirling2(n,k).
a(n) = sum(k=0, n, 6^(n-k) * k! * stirling(n, k, 2));
for(n=0, 15, print1(a(n),", "));

\\ a(n) = (1/7) * Sum_{k=0..n} 7^k * (-6)^(n-k) * k! * Stirling2(n,k) for n > 0.
b(n) = (1/7) * sum(k=0, n, 7^k * (-6)^(n-k) * k! * stirling(n, k, 2));
for(n=0, 50, print1(a(n)-b(n),", "));




