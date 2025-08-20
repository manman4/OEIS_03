\\ a(n) = binomial(n+2,2) * A182401(n).
catalan(n) = binomial(2*n, n)/(n+1);
a182401(n) = sum(k=0, n, 3^(n-k) * binomial(n, k) * catalan(k+1));
a(n) = binomial(n+2, 2) * a182401(n);
for(n=0, 28, print1(a(n),", "));
