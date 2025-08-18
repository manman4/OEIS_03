\\ a(n) = (-3)^n * Sum_{k=0..n} 2^k * binomial(-2/3,k) * binomial(-2/3,n-k).
a(n) = sum(k=0, n, (-3)^n * 2^k * binomial(-2/3, k) * binomial(-2/3, n-k));
for(n=0, 22, print1(a(n),", "));
