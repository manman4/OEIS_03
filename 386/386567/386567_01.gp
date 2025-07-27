\\ a(n) = Sum_{k=0..n-1} binomial(6*k-1,k) * binomial(6*n-6*k,n-k-1).
a(n) = sum(k=0, n-1, binomial(6*k-1, k) * binomial(6*n - 6*k, n - k - 1));
for(n=0, 19, print1(a(n), ", "));
