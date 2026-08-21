M=10000;

a(n) = if(n==0, 0, sum(k=1, sqrtint((n-1)\4), issquare(n-4*k^2)));
for(n=0, 100, print1(a(n), ", "));
\\ for(n=0, M, write("b399169_1.txt", n, " ", a(n)));