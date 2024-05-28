a(n) = -sumdiv(n, d, moebius(7*d)*numdiv(n/d));

for(n=1, 105, print1(a(n), ", ")) 
