\\ a(0) = 1; a(n) = Sum_{k=1..n} binomial(3*k,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(3*k, k)*a(n-k)));
for(n=0, 20, print1(a(n), ", "));

