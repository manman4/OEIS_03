\\ a(n) = Sum_{k=0..n} 5^(n-k) * k! * Stirling2(n,k).
a(n) = sum(k=0, n, 5^(n-k) * k! * stirling(n, k, 2));
for(n=0, 15, print1(a(n),", "));




