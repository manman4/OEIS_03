\\ a(n) = n! * Sum_{k=0..floor(n/4)} binomial(n-k-1,n-4*k)/(24^k * k!).
a(n) = n! * sum(k=0, n\4, binomial(n-k-1,n-4*k)/(24^k * k!));
for(n=0, 21, print1(a(n),", "))  
