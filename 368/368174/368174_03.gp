\\ a(0) = a(1) = a(2) = a(3) = 0; a(n) = binomial(n,3) + Sum_{k=4..n-1} binomial(k,3) * binomial(n-1,k) * a(n-k).
a(n) = if(n<4, 0, binomial(n,3) + sum(k=4, n-1, binomial(k,3) * binomial(n-1, k) * a(n-k)));
for(n=0, 20, print1(a(n), ", "))