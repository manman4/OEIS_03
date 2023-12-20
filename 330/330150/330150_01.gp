\\ a(n) = (-1)^n + Sum_{k=1..n} (-1)^(k-1) * (k-1)! * binomial(n,k) * a(n-k).
a(n) = (-1)^n + sum(k=1, n, (-1)^(k-1) * (k-1)! * binomial(n,k) * a(n-k));
for(n=0, 20, print1(a(n), ", ")); 
