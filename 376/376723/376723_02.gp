\\ a(n) = (1/2) * Sum_{k=0..floor((2*n+5)/4)} binomial(2*k,2*n-4*k+5).
a(n) = 1/2 * sum(k=0, (2*n+5)\4, binomial(2*k, 2*n-4*k+5));
for(n=0, 37, print1(a(n),", "))



