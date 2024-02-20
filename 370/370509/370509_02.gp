\\ a(n) = Sum_{k=0..floor(n/3)} (n-2*k)! * binomila(n-2*k,k).
a(n) = sum(k=0, n\3, (n-2*k)! * binomial(n-2*k,k));
for(n=0, 22, print1(a(n), ", "))