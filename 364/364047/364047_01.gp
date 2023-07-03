a(n) = sumdiv(n, d, (d%6==1)*(-1)^((d-1)/6));
for(n=1, 104, print1(a(n),", "))                                                   
 