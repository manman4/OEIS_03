isok(n) = numdiv(n)>numdiv(n+1) && numdiv(n+1)>numdiv(n+2) && numdiv(n+2)>numdiv(n+3) && numdiv(n+3)>numdiv(n+4);
for(n=1, 181195, if(isok(n), print1(n,", ")))   
