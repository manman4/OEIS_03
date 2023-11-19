\\ a(0) = 1; a(n) = 6*a(n-1) - 3*Sum_{k=1..n-1} (-1)^k * binomial(n-1,k) * a(n-k).
a(n) = if(n==0, 1, 6*a(n-1) - 3*sum(k=1, n-1, (-1)^k * binomial(n-1,k) * a(n-k)));
for(n=0, 17, print1(a(n),", "))   
