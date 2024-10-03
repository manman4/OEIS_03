\\ a(n) = (1/2) * Sum_{k=0..floor((2*n+7)/6)} binomial(2*k,2*n-6*k+7).
a(n) = 1/2 * sum(k=0, (2*n+7)\6, binomial(2*k, 2*n-6*k+7));
for(n=0, 44, print1(a(n),", "))

