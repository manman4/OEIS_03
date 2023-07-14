a(n) = sumdiv(n, d, 3^(n/d-1)*eulerphi(3*d))/(2*n);
for(n=1, 31, print1(a(n),", ")) 