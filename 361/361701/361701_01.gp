M=27;

a(n) = sum(k=0, n\7, (3*k)!/k!^3*binomial(7*k, 3*k)*binomial(n, 7*k));
for(n=0, M, print1(a(n), ", "));
