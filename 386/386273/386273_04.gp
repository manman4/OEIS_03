\\ a(n) = 7^n * Product_{k=1..n} (7 - 3/k).
a(n) = 7^n * prod(k=1, n, 7 - 3/k);
for(n=0, 20, print1(a(n), ", "));
