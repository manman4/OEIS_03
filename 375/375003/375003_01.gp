a(n) = sum(x=1, n, sum(y=1, n, sum(z=1, n, sum(w=1, n, x*y+x*z+x*w+y*z+y*w+z*w==n))));

for(n=1, 83, print1(a(n), ", "))
 

