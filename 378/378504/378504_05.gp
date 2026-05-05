\\ a(n) = ((n+1)/4) * Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+2,k).
a(n) = ((n+1)/4) * sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+2,k));
for(n=0, 50, print1(a(n), ", "));
