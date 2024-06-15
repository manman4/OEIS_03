\\ a(n) = Sum_{k=0..floor(n^2/(2*n+1))} binomial(n * (n-2*k),k).
a(n) = sum(k=0,  n^2\(2*n+1), binomial(n*(n-2*k), k));
for(n=0, 27, print1(a(n),", ")) 
