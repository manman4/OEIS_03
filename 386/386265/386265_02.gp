\\ a(n) = (-1)^n + 2 * Sum_{k=0..n-1} k * a(k) * a(n-1-k).
a(n) = (-1)^n + 2 * sum(k=0, n-1, k * a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "))

