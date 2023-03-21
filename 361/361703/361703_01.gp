M=25;

a(n) = sum(k=0, n\5, (4*k)!/k!^4*binomial(5*k, 4*k)*binomial(n, 5*k));
for(n=0, M, print1(a(n), ", "));
