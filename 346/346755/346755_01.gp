\\ a(n) = n! * Sum_{k=1..floor(n/4)} k^(n-4*k-1)/(24^k * (n-4*k)!).
a(n) = n! * sum(k=1, n\4, k^(n-4*k-1)/(24^k * (n-4*k)!));
for(n=0, 28, print1(a(n), ", ")); 
