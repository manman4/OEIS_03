a(n) = sumdiv(n, d, ((d%7==0)+(d%7==2)+(d%7==5))*d);
for(n=1, 83, print1(a(n), ", "))

