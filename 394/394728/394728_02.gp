\\ a(0) = 1; a(n) = Sum_{k=1..n} binomial(2*n-1,2*k-1) * A001818(k-1) * a(n-k).
a001818(n) = ((2*n)!/(n!*2^n))^2;
a(n) = if(n==0, 1, sum(k=1, n, binomial(2*n-1,2*k-1) * a001818(k-1) * a(n-k)));
for(n=0, 20, print1(a(n),", "));