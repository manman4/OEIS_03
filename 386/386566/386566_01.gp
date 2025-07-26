\\ a(n) = Sum_{k=0..n-1} binomial(5*k-1,k) * binomial(5*n-5*k,n-k-1).
a(n) = sum(k=0, n-1, binomial(5*k-1, k) * binomial(5*n - 5*k, n - k - 1));
for(n=0, 20, print1(a(n), ", ");)
