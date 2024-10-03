\\ a(n) = (1/2) * Sum_{k=0..floor((2*n+9)/8)} binomial(2*k,2*n-8*k+9).
a(n) = 1/2 * sum(k=0, (2*n+9)\8, binomial(2*k, 2*n-8*k+9));
for(n=0, 49, print1(a(n),", "))

