\\ a(n) = 7^n * Product_{k=1..n} (7 - 2/k).
a(n) = 7^n * prod(k=1, n, 7 - 2/k);
for(n=0, 20, print1(a(n), ", "));
