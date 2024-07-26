a(n) = sum(x=1, n, sum(y=1, n, sum(z=1, n, sum(w=1, n, x*y+y*z+z*w+w*x==n))));
for(n=1, 76, print1(a(n),", "))

