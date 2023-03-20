M=22;

a(n) = sum(k=0, n\3, (3*k)!/k!^3*binomial(3*k, k)*binomial(n, 3*k));
for(n=0, M, print1(a(n), ", "));
