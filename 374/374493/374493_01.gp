a000143(n) = if(n==0, 1, 16*sumdiv(n, d, (-1)^(n+d)*d^3));
a(n) = a000143(n^2);
for(n=0, 26, print1(a(n)", "))    