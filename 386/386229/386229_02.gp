\\ a(n) = n + 1 + Sum_{k=0..n-1} (1 + k) * a(k) * a(n-1-k).
a(n) = n + 1 + sum(k=0, n-1, (1 + k) * a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "))

