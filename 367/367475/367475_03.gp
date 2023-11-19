\\ a(0) = 1; a(n) = 2*Sum_{k=1..n} (2*k/n + 1) * (k-1)! * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, 2*sum(k=1, n, (2*k/n + 1) * (k-1)! * binomial(n,k) * a(n-k)));
for(n=0, 17, print1(a(n),", "))   
