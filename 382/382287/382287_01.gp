\\ Irregular triangle T(n,k), n >= 0, 0 <= k <= 2*n+1, read by rows, where T(n,k) = [x^k] (1-x)^(n+1) * Sum_{k=0..n} (k+1)^n * x^k.
T(n, k) = polcoef((1-x)^(n+1)*sum(k=0, n, (k+1)^n*x^k), k);
for(n=0, 6, for(k=0, 2*n+1, print1(T(n, k), ", ")));


