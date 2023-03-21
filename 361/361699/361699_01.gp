M=28;

a(n) = sum(k=0, n\5, binomial(2*k, k)*binomial(5*k, 2*k)*binomial(n, 5*k));
for(n=0, M, print1(a(n), ", "));
