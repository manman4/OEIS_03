M=10000;

a(n) = if(n==0, 0, sum(k=1, sqrtint((n-1)\8), issquare(n-8*k^2)));
for(n=0, 100, print1(a(n), ", "));
\\ for(n=0, M, write("b399211_1.txt", n, " ", a(n)));