a(n) = my(m=sqrtint(n)); sum(i=0, m, sum(j=i, m, i^2+3*i*j+j^2==n));
\\ a(k)=nとなる最小のkを返す
b(n) = my(cnt=0); while(a(cnt)!=n, cnt++); cnt;

for(n=0, 5, print1(b(n), ", "));