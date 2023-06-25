a(n) = sumdiv(7*n-5, d, d%7==6);

for(n=1, 100, print1(a(n),", "))  

b(n) = sumdiv(7*n-5, d, d%7==5);
for(n=1, 100, print1(a(n)-b(n),", "))  
