b(n, k) = sum(i=0, sqrtint(n), sum(j=0, sqrtint(n\k), i^2+k*j^2==n));
a(n, k=5) = my(cnt=0); while(b(cnt, k)!=n, cnt++); cnt;
for(n=0, 7, print1(a(n),", "))