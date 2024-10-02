\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n-2*k,floor(k/3)).
a(n) = sum(k=0, n\2, binomial(n-2*k, k\3));
for(n=0, 54, print1(a(n),", "))


