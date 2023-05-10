b(n) = n! * sum(k=0, n, (n-k)^k/k!);
a(n) = sum(k=0, n, stirling(n, k, 2) * b(k));
for(n=0, 18, print1(a(n),", ")) 