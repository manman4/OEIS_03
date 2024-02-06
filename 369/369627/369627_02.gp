\\ a(n) = Sum_{k=0..floor(n/2)} 9^(n-2*k) * binomial(n-1-5*k/3,n-2*k).
a(n) = sum(k=0, n\2, 9^(n-2*k) * binomial(n-1-5*k/3,n-2*k));
for(n=0, 22, print1(a(n), ", "));   

