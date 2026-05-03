\\ a(n) = Sum_{x_1,x_2, ...,x_6 >= 0 and x_1+x_2+x_3+...+x_6=n} Product_{k=1..6} binomial(5*x_k,x_k).
a(n) = sum(x1=0, n, sum(x2=0, n-x1, sum(x3=0, n-x1-x2, sum(x4=0, n-x1-x2-x3, sum(x5=0, n-x1-x2-x3-x4, binomial(5*x1,x1) * binomial(5*x2,x2) * binomial(5*x3,x3) * binomial(5*x4,x4) * binomial(5*x5,x5) * binomial(5*(n-x1-x2-x3-x4-x5),n-x1-x2-x3-x4-x5))))));
for(n=0, 10, print1(a(n), ", "));