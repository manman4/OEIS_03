\\ a(n) = Sum_{k=0..floor(n/4)} (-1)^k * (n-3*k)! * binomila(n-3*k,k).
a(n) = sum(k=0, n\4, (-1)^k * (n-3*k)! * binomial(n-3*k,k));
for(n=0, 22, print1(a(n), ", "))