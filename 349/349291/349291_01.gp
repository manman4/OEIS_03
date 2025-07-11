\\ a(n) = 1 + Sum_{x_1, x_2, ..., x_6>=0 and x_1+x_2+...+x_6=n-1} Product_{k=1..6} a(x_k). 

a(n) = 1+sum(x1=0, n-1, sum(x2=0, n-1-x1, sum(x3=0, n-1-x1-x2, sum(x4=0, n-1-x1-x2-x3, sum(x5=0, n-1-x1-x2-x3-x4, a(n-1-x1-x2-x3-x4-x5) * a(x1) * a(x2) * a(x3) * a(x4) * a(x5))))));
for(n=0, 8, print1(a(n),", "));
