a(n) = if(n<2, 0, sumdiv(n, d, binomial(d+n/d-3, d)));
for(n=1, 61, print1(a(n),", ")) 
