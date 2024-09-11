\\ a(n) = n! * Sum_{k=1..n} 2^(n-k) * n^(k-1) * binomial(n-1,k-1)/k!.
a(n) = n! * sum(k=1, n, 2^(n-k) * n^(k-1) * binomial(n-1,k-1)/k!);
for(n=0, 17, print1(a(n), ", "))


a088692(n) = n!*polcoef(( (1+2*x)*exp(x + x*O(x^n)) )^(n+1), n, x)/(n+1);
for(n=1, 50, print1(a(n)-n * a088692(n-1), ", "))
