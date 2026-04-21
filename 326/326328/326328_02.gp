\\ a(0) = 1; a(n) = -(1/4) * Sum_{k=0..n-1} (3 + 9^(n-k)) * binomial(2*n,2*k) * a(k).
a(n) = if(n==0, 1, -(1/4) * sum(k=0, n-1, (3 + 9^(n-k)) * binomial(2*n, 2*k) * a(k)));
for(n=0, 15, print1(a(n), ", "));
