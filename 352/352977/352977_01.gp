\\ a(n) = (-1)^n * (25^n + 1)/2 - Sum_{k=0..n-1} (-36)^(n-k) * binomial(2*n,2*k) * a(k). 
a(n) = (-1)^n * (25^n + 1)/2 - sum(k=0, n-1, (-36)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 12, print1(a(n),", "));
