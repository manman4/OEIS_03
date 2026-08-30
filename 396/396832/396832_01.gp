\\ a(n) = Product_{k=1..n} (n - k + 2 - ceiling((n-1)/k)) unless n = 1.

a(n) = if(n==1, 1, prod(k=1, n, n-k+2-ceil((n-1)/k)));
for(n=0, 25, print1(a(n),", "));