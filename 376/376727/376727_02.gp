\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*k+1,2*n-6*k+1).
a(n) = sum(k=0, n\3, binomial(2*k+1, 2*n-6*k+1));
for(n=0, 44, print1(a(n),", "))



