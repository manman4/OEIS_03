\\ a(n) = (-3)^n * Sum_{k=0..n} 2^k * binomial(-1/3,k) * binomial(-1/3,n-k).
a(n) = sum(k=0, n, (-3)^n * 2^k * binomial(-1/3, k) * binomial(-1/3, n-k));
for(n=0, 23, print1(a(n),", "));
