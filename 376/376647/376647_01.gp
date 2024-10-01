\\ a(n) = Sum_{k=0..floor(n/3)} binomial(floor(k/2),n-3*k).
a(n) = sum(k=0, n\3, binomial(k\2, n-3*k));
for(n=0, 71, print1(a(n),", "))


