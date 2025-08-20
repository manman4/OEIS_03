\\ a(n) = binomial(n+2,2) * A005572(n).
a005572(n) = sum(k=0, n, binomial(n, k) * 2^(n-k) * binomial(2*k+2, k)/(k+1) );
a(n) = binomial(n+2, 2) * a005572(n);
for(n=0, 28, print1(a(n),", "));
