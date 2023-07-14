a(n) = sumdiv(n, d, 5^(n/d-1)*eulerphi(5*d))/(4*n);
for(n=1, 27, print1(a(n),", ")) 