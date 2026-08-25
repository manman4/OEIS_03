M=10000;

a(n, k=9) = sum(i=0, sqrtint(n), sum(j=0, sqrtint(n\k), i^2+k*j^2==n));
\\ for(n=0, 100, print1(a(n), ", "));
for(n=0, M, write("b399270_1.txt", n, " ", a(n)));
