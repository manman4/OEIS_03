a(n) = if(n==0, 0, sum(k=1, sqrtint((n-1)\11), issquare(n-11*k^2)));
for(n=0, 101, print1(a(n),", "))      