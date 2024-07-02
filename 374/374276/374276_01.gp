a(n) = my(m=sqrtint(n)); sum(i=0, m, sum(j=i, m, i^2+3*i*j+j^2==n));
for(n=0, 121, print1(a(n), ", "));