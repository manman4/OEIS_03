\\ a(0) = 1; a(n) = Sum_{k=1..n} (Product_{j=0..k-1} (-5*j+1)) * binomial(n-1,k-1) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, prod(j=0, k-1, -5*j+1) * binomial(n-1, k-1) * a(n-k)));
for(n=0, 20, print1(a(n), ", "))
