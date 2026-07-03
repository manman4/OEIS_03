\\ a(1) = 1; a(n) = (n+1) * Sum_{k=1..n-1} a(k)*a(n-k).
a(n) = if(n==1, 1, (n+1) * sum(k=1, n-1, a(k)*a(n-k)));
for(n=1, 15, print1(a(n), ", "))
