a(n) = sumdiv(n, d, 7^(n/d-1)*eulerphi(7*d))/(6*n);
for(n=1, 24, print1(a(n),", ")) 