b(n) = n! * sum(k=0, n, (n-k)^k/k!);
a(n) = sum(k=0, n, abs(stirling(n, k, 1)) * b(k));
for(n=0, 20, print1(a(n),", ")) 