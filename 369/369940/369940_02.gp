\\ a(n) = Sum_{k=0..floor(n/3)} 9^(n-3*k) * binomial(n-1-8*k/3,n-3*k).
a(n) = sum(k=0, n\3, 9^(n-3*k) * binomial(n-1-8*k/3,n-3*k));
for(n=0, 23, print1(a(n), ", "));   

