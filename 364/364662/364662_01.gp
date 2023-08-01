isok(n) = sigma(n)<sigma(n+1) && sigma(n+1)<sigma(n+2) && sigma(n+2)<sigma(n+3);
for(n=1, 10000, if(isok(n), print1(n,", ")))   
