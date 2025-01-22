\\ a(0) = a(1) = a(2) = 0; a(n) = n!/(2*(n-2)) + Sum_{k=3..n-1} k!/(2*(k-2)) * binomial(n-1,k) * a(n-k).
a(n) = if(n<3, 0, n!/(2*(n-2)) + sum(k=3, n-1, k!/(2*(k-2)) * binomial(n-1, k) * a(n-k)));
for(n=0, 20, print1(a(n), ", "))
