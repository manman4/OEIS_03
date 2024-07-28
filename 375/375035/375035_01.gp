M=100;
a(n) = sum(w=1, n\2, sum(x=1, n\(2*w), sum(y=1, n\x, sum(z=1, n\y, sum(u=0, n\z, 2*w*x+x*y+y*z+z*u+u*x==n)))));
for(n=1, M, print1(a(n), ", "))

b(n) = (sigma(n, 2) + 4*sigma(n) - (4*n+1)*sigma(n, 0))/8;
for(n=1, M, print1(a(2*n-1)-b(2*n-1), ", "))
