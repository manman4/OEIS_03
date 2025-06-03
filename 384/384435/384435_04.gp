\\ a(n) = Sum_{k=0..n} 3^k * 2^(n-k) * k! * Stirling2(n,k).
a(n) = sum(k=0, n, 3^k * 2^(n-k) * k! * stirling(n, k, 2));
for(n=0, 15, print1(a(n),", "));

\\ a(n) = (3/5) * A201366(n) 
for(n=0, 15, print1((5/3) * a(n),", "));

\\ a(n) = (3/5) * Sum_{k=0..n} 5^k * (-2)^(n-k) * k! * Stirling2(n,k) for n > 0.
b(n) = (3/5) * sum(k=0, n, 5^k * (-2)^(n-k) * k! * stirling(n, k, 2));
for(n=0, 50, print1(a(n)-b(n),", "));




