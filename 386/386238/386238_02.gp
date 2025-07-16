\\ a(n) = (-1)^n + Sum_{k=0..n-1} (1 + k) * a(k) * a(n-1-k).
a(n) = (-1)^n + sum(k=0, n-1, (1 + k) * a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "))

