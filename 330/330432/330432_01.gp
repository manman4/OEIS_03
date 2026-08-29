\\ a(n) = Product_{k=1..n} (n - k + 2 - ceiling(n/k)).

a(n) = prod(k=1, n, n-k+2-ceil(n/k));
for(n=0, 24, print1(a(n),", "));