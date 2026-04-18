\\ a(n) = (-1)^n * ((3+sqrt(2))^(2*n) + (3-sqrt(2))^(2*n))/2 - Sum_{k=0..n-1} (-18)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * ((3+sqrt(2))^(2*n) + (3-sqrt(2))^(2*n))/2 - sum(k=0, n-1, (-18)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 12, print1(round(a(n)),", "));
