a(n) = my(r=sqrtint(n)); sum(i=-r, r, sum(j=-r, r, sum(k=-r, r, sum(l=-r, r, (i^2+j^2+k^2+l^2<=n)*(i^2+j^2+k^2+l^2) ))))/8;

for(n=0, 47, print1(a(n)", "))    