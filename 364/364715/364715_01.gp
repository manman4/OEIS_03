isok(n) = numdiv(n)<numdiv(n+1) && numdiv(n+1)<numdiv(n+2);
for(n=1, 1027, if(isok(n), print1(n,", ")))   
