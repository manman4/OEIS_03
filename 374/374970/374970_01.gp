a(n) = sum(x=1, n, sum(y=1, n, sum(z=1, n, sum(w=1, n, (gcd([x, y, z, w])==1)*(x*y+y*z+z*w+w*x==n)))));
for(n=1, 80, print1(a(n),", "))

