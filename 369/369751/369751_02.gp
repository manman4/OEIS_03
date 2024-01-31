\\ a(0) = 1; a(n) = -3 * (n-1)! * Sum_{k=1..min(3,n)} binomial(2,k-1) * a(n-k)/(n-k)!.
a(n) = if(n==0, 1, -3 * (n-1)! * sum(k=1, min(3,n), binomial(2,k-1) * a(n-k)/(n-k)!));
for(n=0, 22, print1(a(n), ", "));