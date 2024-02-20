\\ a(n) = Sum_{k=0..floor(n/3)} (n-3*k)! * binomila(n-2*k-1,k).
a(n) = sum(k=0, n\3, (n-3*k)! * binomial(n-2*k-1,k));
for(n=0, 22, print1(a(n), ", "))