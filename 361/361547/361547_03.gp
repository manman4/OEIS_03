\\ a(n) = n! * Sum_{k=0..floor(n/5)} binomial(n-4*k-1,n-5*k)/(120^k * k!).
a(n) = n! * sum(k=0, n\5, binomial(n-4*k-1,n-5*k)/(120^k * k!));
for(n=0, 23, print1(a(n),", "))  
