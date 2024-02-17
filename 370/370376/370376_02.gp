\\ a(0) = 1; a(n) = 1/3 * Sum_{k=1..n} binomial(3*k,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, binomial(3*k, k)*a(n-k)/3));
for(n=0, 22, print1(a(n), ", "));

