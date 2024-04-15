\\ a(n) = 9^n*sum(k=0, n, binomial(n, k)*binomial(k/3+1/3, n)/(k+1));
a(n) = 9^n*sum(k=0, n, binomial(n, k)*binomial(k/3+1/3, n)/(k+1));
for(n=0, 21, print1(a(n), ", "))
