a(n) = my(k=1); while(binomial(k^2, k)%n^2>0, k++); k;
for(n=1, 79, print1(a(n),", "))  
