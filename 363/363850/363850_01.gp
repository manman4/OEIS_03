a(n) = sumdiv(7*n-3, d, d%7==1);

for(n=1, 100, print1(a(n),", "))  

b(n) = sumdiv(7*n-3, d, d%7==4);
for(n=1, 100, print1(a(n)-b(n),", "))  
