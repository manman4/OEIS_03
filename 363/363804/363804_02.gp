a(n) = sumdiv(n, d, ((d%7==0)+(d%7==3)+(d%7==4))*d);
for(n=1, 83, print1(a(n), ", "))

