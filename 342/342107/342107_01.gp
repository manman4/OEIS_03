M=20;

a(n) = sum(k=0, n, (4*k)!/k!^4);
for(n=0, M, print1(a(n), ", "));
