a(n) = sumdiv(n, d, (d%6==5)*(-1)^((d-5)/6));
for(n=1, 102, print1(a(n),", "))                                                   
 