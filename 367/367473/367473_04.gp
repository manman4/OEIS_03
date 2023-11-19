\\ a(0) = 1; a(n) = 9*a(n-1) - 4*Sum_{k=1..n-1} (-1)^k * binomial(n-1,k) * a(n-k).
a(n) = if(n==0, 1, 9*a(n-1) - 4*sum(k=1, n-1, (-1)^k * binomial(n-1,k) * a(n-k)));
for(n=0, 17, print1(a(n),", "))   
