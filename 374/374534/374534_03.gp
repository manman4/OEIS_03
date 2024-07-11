a(n) = my(r=sqrtint(n)); sum(i=-r, r, sum(j=-r, r, (i^2+j^2<=n)*(i^2+j^2) ))/4;

for(n=0, 59, print1(a(n)", "))    