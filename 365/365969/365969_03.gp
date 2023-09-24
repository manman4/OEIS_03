a(n) = if(n<5, 1, a(n-1) + 120 * binomial(n-1,5) * a(n-5) );                   
for(n=0, 23, print1(a(n),", "))  
