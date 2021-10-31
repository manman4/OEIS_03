M=15;

a(n) = sum(a=0, n, sum(b=a, n, sum(c=b, n, sum(d=c, n, if(a+b+c+d==n, (3*n)!/((n-a)!*(n-b)!*(n-c)!*(n-d)!), 0)))));
for(n=0, M, print1(a(n), ", "));