a(n) = sumdiv(n, d, (d%3==2)*(-1)^d); 

for(n=1, 97, print1(a(n),", "))  

b(n) = sumdiv(n, d, (n/d%3==2)*(-1)^(n/d));

\\ 一致することの確認
for(n=1, 500, print1(a(n)-b(n),", "))  
