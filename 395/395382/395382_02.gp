\\ a(0) = 1; a(n) = -(1/8) * Sum_{k=0..n-1} (-1)^(n-k) * (4^(n-k+1) + 16^(n-k)) * binomial(2*n,2*k) * a(k).
a(n) = if(n==0, 1, -(1/8) * sum(k=0, n-1, (-1)^(n-k) * (4^(n-k+1) + 16^(n-k)) * binomial(2*n, 2*k) * a(k)));
for(n=0, 15, print1(a(n), ", "));
