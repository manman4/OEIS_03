\\ a(n) = binomial(n+2,2) * A386362(n).
a386362(n) = sum(k=0, n, 3^k * binomial(n,k) * binomial(2*(k+1),k+1)/(k+2));
a(n) = binomial(n+2, 2) * a386362(n);
for(n=0, 28, print1(a(n),", "));
    