\\ a(n) = Sum_{k=0..floor(n/3)} binomial(n-3*k,floor(k/3)).
a(n) = sum(k=0, n\3, binomial(n-3*k, k\3));
for(n=0, 58, print1(a(n),", "))



